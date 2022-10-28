// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/GLM.h"
#include "base/Math/BitMath.h"
#include "base/CompileTime/Math.h"

namespace AE::Math
{

	//
	// Packed Integer
	//

	template <typename T>
	struct PackedInt
	{
		STATIC_ASSERT( IsUnsignedInteger<T> );

	// types
	public:
		using Self		= PackedInt<T>;
		using Value_t	= T;
		using HalfInt_t	= ByteSizeToUInt< sizeof(T)/2 >;

		STATIC_ASSERT( sizeof(Value_t) == sizeof(HalfInt_t)*2 );

		static constexpr uint	_Offset	= CT_SizeOfInBits< HalfInt_t >;
		static constexpr uint	_Mask	= ToBitMask< HalfInt_t >( _Offset );


	// variables
	public:
		HalfInt_t	hi	= 0;
		HalfInt_t	lo	= 0;


	// methods
	public:
		constexpr PackedInt () {}
		constexpr PackedInt (const Self &) = default;

		constexpr PackedInt (Value_t val) : 
			hi{HalfInt_t( val >> _Offset )},
			lo{HalfInt_t{ val & _Mask }}
		{}

		Self&  operator = (const Self &) = default;

		Self&  operator = (Value_t val)
		{
			hi = HalfInt_t( val >> _Offset );
			lo = HalfInt_t( val & _Mask );
			return *this;
		}

		ND_ explicit operator Value_t () const { return (Value_t{hi} << _Offset) | Value_t{lo}; }
	};

	using packed_ulong	= PackedInt<ulong>;


} // AE::Math
