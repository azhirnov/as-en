// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/GLM.h"
#include "base/Algorithms/Cast.h"

namespace AE::Math
{
	
	//
	// Unsigned Float 8 bit
	//

	struct UFloat8
	{
	// types
	public:
		using Self	= UFloat8;

	private:
		enum class EValue : ubyte {};
		
		// ufloat8
		static constexpr uint	_HMaxExp	= 7;
		static constexpr uint	_HManBits	= 4;
		static constexpr uint	_HExpBits	= 4;
		static constexpr int	_HMaxExpAbs	= (1u << _HExpBits) - 1;

		// float32
		static constexpr uint	_FMaxExp	= 127;
		static constexpr uint	_FManBits	= 23;
		static constexpr uint	_FExpBits	= 8;

		// {float32} - {ufloat8}
		static constexpr uint	_MaxExpFmH	= _FMaxExp - _HMaxExp;
		static constexpr uint	_ManBitsFmH	= _FManBits - _HManBits;


		struct Float8Bits
		{
			ubyte	m	: _HManBits;	// mantissa bits
			ubyte	e	: _HExpBits;	// exponent	bits (-7 .. +8)
		};
		
		union FloatBits
		{
			struct
			{
				uint	m	: _FManBits;	// mantissa bits
				uint	e	: _FExpBits;	// exponent bits (-127 .. +128)
				uint	s	: 1;			// sign bit
			}		bits;
			float	f;
			uint	i;

			constexpr FloatBits () : f{0.f} {}
		};

		STATIC_ASSERT( sizeof(ubyte) == sizeof(Float8Bits) );


	// variables
	private:
		union {
			Float8Bits		_bits;
			ubyte			_value;
		};


	// methods
	private:
		constexpr explicit UFloat8 (EValue val)			__NE___	: _value{ubyte(val)}		{}

	public:
		constexpr UFloat8 ()							__NE___	: _value{0}					{}

		constexpr UFloat8 (const Self &other)			__NE___	: _value{ other._value }	{}
		explicit UFloat8 (const float &f)				__NE___	{ Set( f ); }

			constexpr Self&  operator = (Self rhs)		__NE___	{ _value = rhs._value;  return *this; }

		ND_ constexpr bool  operator == (Self rhs)		C_NE___	{ return _value == rhs._value; }

		// set/get
		ND_ constexpr ubyte		GetU ()					C_NE___	{ return _value; }
		
		ND_ constexpr float		Get ()					C_NE___;
			constexpr Self&		Set (const float &f)	__NE___;

		ND_	constexpr float		GetFast ()				C_NE___;
			constexpr Self&		SetFast (float f)		__NE___;

		ND_ explicit operator float ()					C_NE___	{ return Get(); }
		
		ND_ static constexpr Self  Min ()				__NE___	{ return Self{EValue(0x01)}; }	// 1.5e-5
		ND_ static constexpr Self  Max ()				__NE___	{ return Self{EValue(0xEF)}; }	// 248
		ND_ static constexpr Self  Inf ()				__NE___	{ return Self{EValue(0xF0)}; }
		ND_ static constexpr Self  NaN ()				__NE___	{ return Self{EValue(0xFF)}; }
		ND_ static constexpr float Epsilon ()			__NE___	{ return 2.0e-3f; }
	};
	
/*
=================================================
	Set
=================================================
*/
	forceinline constexpr UFloat8&  UFloat8::Set (const float &f) __NE___
	{
		FloatBits	entry;
		entry.f = f;

		int	e = ((entry.i >> _FManBits)	& 0x000000ff) - _MaxExpFmH;
		int	m =   entry.i				& 0x007fffff;

		if ( entry.bits.s == 1 )
		{
			_value = 0;		// overflow
			return *this;
		}
		else
		if ( e <= 0 )
		{
			if ( e < -10 )
			{
				_value = 0;
				return *this;
			}

			m = (m | 0x00800000) >> (1 - e);

			if ( m & 0x00001000 )
				m += 0x00002000;

			_value = ubyte(m >> _ManBitsFmH);
			return *this;
		}
		else
		if ( e == 0xff - _MaxExpFmH )
		{
			if ( m == 0 )
			{
				_value = Inf()._value;
				return *this;
			}
			else
			{
				m >>= _ManBitsFmH;

				_value = ubyte(Inf()._value | m | (m == 0));
				return *this;
			}
		}
		else
		{
			if ( m & 0x00001000 )
			{
				m += 0x00002000;

				if ( m & 0x00800000 )
				{
					m =  0;     // overflow in significand,
					e += 1;     // adjust exponent
				}
			}
			
			if ( e >= _HMaxExpAbs )
			{
				_value = Inf()._value;	// overflow
				return *this;
			}

			_value = ubyte((e << _HManBits) | (m >> _ManBitsFmH));
			return *this;
		}
	}
	
/*
=================================================
	Get
=================================================
*/
	forceinline constexpr float  UFloat8::Get () C_NE___
	{
		int e = _bits.e;
		int m = _bits.m;

		if ( e == 0 )
		{
			if ( m == 0 )
			{
				// Plus or minus zero
				return 0.f;
			}
			else
			{
				// Denormalized number -- renormalize it
				while ( not (m & 0x00000400))
				{
					m <<= 1;
					e -=  1;
				}

				e += 1;
				m &= ~0x00000400;
			}
		}
		else
		if ( e == _HMaxExpAbs )
		{
			if ( m == 0 )
			{
				// Positive or negative infinity
				FloatBits	result;
				result.i = uint(0x7f800000);
				return result.f;
			}
			else
			{
				// Nan -- preserve sign and significand bits
				FloatBits	result;
				result.i = uint(0x7f800000 | (m << _ManBitsFmH));
				return result.f;
			}
		}

		// Normalized number
		e = e + _MaxExpFmH;
		m = m << _ManBitsFmH;

		// Assemble e and m.
		FloatBits	result;
		result.i = uint((e << _FManBits) | m);
		return result.f;
	}

/*
=================================================
	GetFast
=================================================
*/
	forceinline constexpr float  UFloat8::GetFast () C_NE___
	{
		FloatBits	f;
		f.bits.e = _bits.e == 0 ? 0 : _bits.e + _MaxExpFmH;
		f.bits.m = _bits.m << _ManBitsFmH;
		f.bits.s = 0;
		return f.f;
	}
	
/*
=================================================
	SetFast
=================================================
*/
	forceinline constexpr UFloat8&  UFloat8::SetFast (float val) __NE___
	{
		FloatBits	f;
		f.f		= val;
		ASSERT( f.bits.s == 0 );

		_bits.m = (f.bits.m + (1u << _ManBitsFmH) - 1) >> _ManBitsFmH;
		_bits.e = f.bits.e == 0 ? 0 : f.bits.e - _MaxExpFmH;
		return *this;
	}


} // AE::Math


namespace AE::Base
{
	template <>	struct TMemCopyAvailable< UFloat8 >		{ static constexpr bool  value = true; };
	template <>	struct TZeroMemAvailable< UFloat8 >		{ static constexpr bool  value = true; };
	template <>	struct TTriviallySerializable< UFloat8 >{ static constexpr bool  value = true; };

	namespace _hidden_
	{
		template <> struct _IsScalar< AE::Math::UFloat8 >		{ static constexpr bool  value = true; };
		template <> struct _IsFloatPoint< AE::Math::UFloat8 >	{ static constexpr bool  value = true; };
		template <> struct _IsSigned< AE::Math::UFloat8 >		{ static constexpr bool  value = false; };
	}

} // AE::Base
//-----------------------------------------------------------------------------


namespace std
{
	template <>
	class numeric_limits< AE::Math::UFloat8 > final
	{
	public:
		static constexpr bool	is_signed		= false;
		static constexpr bool	is_specialized	= true;
		static constexpr int	radix			= 2;
		static constexpr int	digits			= 4;	// TODO: check
		static constexpr int	digits10		= 3;	// TODO: check
		static constexpr int	max_digits10	= 1;	// TODO: check
		static constexpr int	max_exponent	= 7;
		static constexpr int	max_exponent10	= 1;	// TODO: check
		static constexpr int	min_exponent	= -7;
		static constexpr int	min_exponent10	= -1;	// TODO: check

		ND_ static constexpr AE::Math::UFloat8  min () __NE___ { return AE::Math::UFloat8::Min(); }

		ND_ static constexpr AE::Math::UFloat8  max () __NE___ { return AE::Math::UFloat8::Max(); }
	};

} // std
