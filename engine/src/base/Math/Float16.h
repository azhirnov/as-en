// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/GLM.h"

namespace AE::Math
{

	//
	// Half (Float16)
	//

	struct Float16
	{
	// types
	public:
		using Self	= Float16;

	private:
		struct HalfBits
		{
			ushort	m	: 10;	// mantissa
			ushort	e	: 5;	// exponent
			ushort	s	: 1;	// sign
		};
		
		union FloatBits
		{
			struct
			{
				uint	m	: 23;	// mantissa bits
				uint	e	: 8;	// exponent bits
				uint	s	: 1;	// sign bit
			}		bits;
			float	f;

			FloatBits () : f{0.f} {}
		};

		STATIC_ASSERT( sizeof(glm::detail::hdata) == sizeof(HalfBits) );


	// variables
	private:
		union {
			HalfBits			_bits;
			glm::detail::hdata	_value;
		};


	// methods
	public:
		Float16 () : _value{0} {}

		Float16 (const Float16 &other) : _value{ other._value } {}
		explicit Float16 (const float &f) : _value{ glm::detail::toFloat16( f )} {}

		Float16&  operator = (Float16 rhs)			{ _value = rhs._value;  return *this; }

		ND_ bool  operator == (Float16 rhs)	const	{ return _value == rhs._value; }

		// set/get
		ND_ ushort	GetU ()			const	{ return _value; }
		ND_ float	Get ()			const	{ return glm::detail::toFloat32( _value ); }
			void	Set (const float &f)	{ _value = glm::detail::toFloat16( f ); }

		ND_	float	GetFast ()		const;
			void	SetFast (float f);

		ND_ explicit operator float () const	{ return Get(); }
	};

	using half = Float16;

	
/*
=================================================
	GetFast
=================================================
*/
	forceinline float  Float16::GetFast () const
	{
		FloatBits	f;
		f.bits.s = _bits.s;
		f.bits.e = _bits.e + (127 - 15);
		f.bits.m = _bits.m << (23 - 10);
		return f.f;
	}
	
/*
=================================================
	SetFast
=================================================
*/
	forceinline void  Float16::SetFast (float val)
	{
		FloatBits	f;
		f.f		= val;
		_bits.m = (f.bits.m + (1u << 13) - 1) >> (23 - 10);
		_bits.e = f.bits.e - (127 - 15);
		_bits.s = f.bits.s;
	}

} // AE::Math
