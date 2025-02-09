// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "graphics_rhi/Public/Common.h"

namespace AE::Graphics
{

	//
	// Multi Samples
	//

	struct MultiSamples
	{
	// variables
	private:
		ubyte	_value	= 0;	// power of 2


	// methods
	public:
		__Cx__ MultiSamples ()								__NE___ {}

		__Cx__ explicit MultiSamples (PowerOfTwo pot)		__NE___ : _value{ubyte(pot)} {}

		template <typename T, ENABLEIF( IsUnsignedInteger<T> )>
		explicit MultiSamples (T samples)					__NE___ : _value{ CheckCast<ubyte>(IntLog2( samples ))}
		{
			ASSERT( IsPowerOfTwo( samples ));
		}

		NdCx__ uint	 Get ()									C_NE___	{ return 1u << _value; }
		NdCx__ uint	 GetPowerOf2 ()							C_NE___	{ return _value; }

		NdCx__ bool	 IsEnabled ()							C_NE___	{ return _value > 1; }

		NdCx__ bool	 operator == (const MultiSamples &rhs)	C_NE___	{ return _value == rhs._value; }
		NdCx__ bool	 operator != (const MultiSamples &rhs)	C_NE___	{ return _value != rhs._value; }
		NdCx__ bool	 operator >  (const MultiSamples &rhs)	C_NE___	{ return _value >  rhs._value; }
		NdCx__ bool	 operator <  (const MultiSamples &rhs)	C_NE___	{ return _value <  rhs._value; }
		NdCx__ bool	 operator >= (const MultiSamples &rhs)	C_NE___	{ return _value >= rhs._value; }
		NdCx__ bool	 operator <= (const MultiSamples &rhs)	C_NE___	{ return _value <= rhs._value; }
	};


	Nd__In MultiSamples operator "" _samples (unsigned long long value) __NE___	{ return MultiSamples( uint(value) ); }


} // AE::Graphics

namespace AE::Base
{
	template <> struct TMemCopyAvailable< AE::Graphics::MultiSamples >		: CT_True {};
	template <> struct TZeroMemAvailable< AE::Graphics::MultiSamples >		: CT_True {};
	template <> struct TTriviallySerializable< AE::Graphics::MultiSamples >	: CT_True {};

} // AE::Base


template <>
struct std::hash< AE::Graphics::MultiSamples >
{
	ND_ size_t  operator () (const AE::Graphics::MultiSamples &value) C_NE___
	{
		return size_t(AE::Base::HashOf( value.Get() ));
	}
};
