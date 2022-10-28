// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/GLM.h"
#include "base/Algorithms/Cast.h"

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
		enum class EValue : ushort {};

		struct HalfBits
		{
			ushort	m	: 10;	// mantissa bits
			ushort	e	: 5;	// exponent	bits (-14 .. +15)
			ushort	s	: 1;	// sign bits
		};
		
		union FloatBits
		{
			struct
			{
				uint	m	: 23;	// mantissa bits
				uint	e	: 8;	// exponent bits (-127 .. +128)
				uint	s	: 1;	// sign bit
			}		bits;
			float	f;

			constexpr FloatBits () : f{0.f} {}
		};

		STATIC_ASSERT( sizeof(glm::detail::hdata) == sizeof(HalfBits) );
		STATIC_ASSERT( sizeof(glm::detail::hdata) == sizeof(ushort) );


	// variables
	private:
		union {
			HalfBits			_bits;
			glm::detail::hdata	_value;
			ushort				_valueU;
		};


	// methods
	private:
		constexpr explicit Float16 (EValue val) : _valueU{ushort(val)} {}

	public:
		constexpr Float16 () : _value{0} {}

		constexpr Float16 (const Self &other) : _value{ other._value } {}
		explicit Float16 (const float &f) : _value{ glm::detail::toFloat16( f )} {}

			constexpr Self&  operator = (Self rhs)				{ _value = rhs._value;  return *this; }

		ND_ constexpr bool  operator == (Self rhs)		const	{ return _value == rhs._value; }

		// set/get
		ND_ constexpr ushort	GetU ()					const	{ return _valueU; }

		ND_ float				Get ()					const	{ return glm::detail::toFloat32( _value ); }
			Self&				Set (const float &f)			{ _value = glm::detail::toFloat16( f );  return *this; }

		ND_	constexpr float		GetFast ()				const;
			constexpr Self&		SetFast (float f);

		ND_ explicit operator float ()	const		{ return Get(); }

	  #ifdef AE_SIMD_NEON
		ND_ explicit operator float16_t () const	{ return BitCast<float16_t>( _valueU ); }
	  #endif
		
		ND_ static constexpr Self  Min ()		{ return Self{EValue(0x0001)}; }	// 5.9e-8
		ND_ static constexpr Self  Max ()		{ return Self{EValue(0x7BFF)}; }	// 65504
		ND_ static constexpr Self  Inf ()		{ return Self{EValue(0x7C00)}; }
		ND_ static constexpr Self  NegInf ()	{ return Self{EValue(0xFC00)}; }
		ND_ static constexpr float Epsilon ()	{ return 2.0e-10f; }
	};

	using half = Float16;

/*
=================================================
	GetFast
=================================================
*/
	forceinline constexpr float  Float16::GetFast () const
	{
		FloatBits	f;
		f.bits.s = _bits.s;
		f.bits.e = _bits.e == 0 ? 0 : _bits.e + (127 - 15);
		f.bits.m = _bits.m << (23 - 10);
		return f.f;
	}
	
/*
=================================================
	SetFast
=================================================
*/
	forceinline constexpr Float16&  Float16::SetFast (float val)
	{
		FloatBits	f;
		f.f		= val;
		_bits.m = (f.bits.m + (1u << (23 - 10)) - 1) >> (23 - 10);
		_bits.e = f.bits.e == 0 ? 0 : f.bits.e - (127 - 15);
		_bits.s = f.bits.s;
		return *this;
	}
//-----------------------------------------------------------------------------


	
	//
	// Unsigned 16 bit Float
	//

	struct UFloat16
	{
	// types
	public:
		using Self	= UFloat16;

	private:
		enum class EValue : ushort {};
		
		// ufloat16
		static constexpr uint	_HMaxExp	= 31;
		static constexpr uint	_HManBits	= 10;
		static constexpr uint	_HExpBits	= 6;
		static constexpr int	_HMaxExpAbs	= (1u << _HExpBits) - 1;

		// float32
		static constexpr uint	_FMaxExp	= 127;
		static constexpr uint	_FManBits	= 23;
		static constexpr uint	_FExpBits	= 8;

		// {float32} - {ufloat16}
		static constexpr uint	_MaxExpFmH	= _FMaxExp - _HMaxExp;
		static constexpr uint	_ManBitsFmH	= _FManBits - _HManBits;

		struct HalfBits
		{
			ushort	m	: _HManBits;	// mantissa bits
			ushort	e	: _HExpBits;	// exponent	bits (-31 .. +32)
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

		STATIC_ASSERT( sizeof(ushort) == sizeof(HalfBits) );
		STATIC_ASSERT( sizeof(float) == sizeof(FloatBits) );


	// variables
	private:
		union {
			HalfBits	_bits;
			ushort		_value;
		};


	// methods
	private:
		constexpr explicit UFloat16 (EValue val) : _value{ushort(val)} {}

	public:
		constexpr UFloat16 () : _value{0} {}

		constexpr UFloat16 (const Self &other) : _value{ other._value } {}
		explicit UFloat16 (const float &f) { Set( f ); }

			constexpr Self&  operator = (Self rhs)			{ _value = rhs._value;  return *this; }

		ND_ constexpr bool  operator == (Self rhs)	const	{ return _value == rhs._value; }

		// set/get
		ND_ constexpr ushort	GetU ()					const	{ return _value; }
		
		ND_ constexpr float		Get ()					const;
			constexpr Self&		Set (const float &f);

		ND_	constexpr float		GetFast ()				const;
			constexpr Self&		SetFast (float f);

		ND_ explicit operator float ()	const	{ return Get(); }

		ND_ static constexpr Self  Min ()		{ return Self{EValue(0x0001)}; }	// 9.09e-13
		ND_ static constexpr Self  Max ()		{ return Self{EValue(0xFBFF)}; }	// 4 292 870 144
		ND_ static constexpr Self  Inf ()		{ return Self{EValue(0xFC00)}; }
		ND_ static constexpr Self  NaN ()		{ return Self{EValue(0xFFFF)}; }
		ND_ static constexpr float Epsilon ()	{ return 2.0e-10f; }
	};
	
	using uhalf = UFloat16;

	
/*
=================================================
	Set
=================================================
*/
	forceinline constexpr UFloat16&  UFloat16::Set (const float &f)
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

			_value = ushort(m >> _ManBitsFmH);
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

				_value = ushort(Inf()._value | m | (m == 0));
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

			_value = ushort((e << _HManBits) | (m >> _ManBitsFmH));
			return *this;
		}
	}
	
/*
=================================================
	Get
=================================================
*/
	forceinline constexpr float  UFloat16::Get () const
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
				while ( !(m & 0x00000400))
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
	forceinline constexpr float  UFloat16::GetFast () const
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
	forceinline constexpr UFloat16&  UFloat16::SetFast (float val)
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
	template <>	struct TMemCopyAvailable< Float16 >			{ static constexpr bool  value = true; };
	template <>	struct TZeroMemAvailable< Float16 >			{ static constexpr bool  value = true; };
	template <>	struct TTrivialySerializable< Float16 >		{ static constexpr bool  value = true; };
	
	template <>	struct TMemCopyAvailable< UFloat16 >		{ static constexpr bool  value = true; };
	template <>	struct TZeroMemAvailable< UFloat16 >		{ static constexpr bool  value = true; };
	template <>	struct TTrivialySerializable< UFloat16 >	{ static constexpr bool  value = true; };

	namespace _hidden_
	{
		template <> struct _IsScalar< AE::Math::Float16 >		{ static constexpr bool  value = true; };
		template <> struct _IsFloatPoint< AE::Math::Float16 >	{ static constexpr bool  value = true; };
		
		template <> struct _IsScalar< AE::Math::UFloat16 >		{ static constexpr bool  value = true; };
		template <> struct _IsFloatPoint< AE::Math::UFloat16 >	{ static constexpr bool  value = true; };
	}

} // AE::Base
//-----------------------------------------------------------------------------


namespace std
{
	template <>
	class numeric_limits< AE::Math::Float16 > final
	{
	public:
		static constexpr bool	is_signed		= true;
		static constexpr bool	is_specialized	= true;
		static constexpr int	radix			= 2;
		static constexpr int	digits			= 11;
		static constexpr int	digits10		= 3;	// TODO: check
		static constexpr int	max_digits10	= 1;	// TODO: check
		static constexpr int	max_exponent	= 16;
		static constexpr int	max_exponent10	= 1;	// TODO: check
		static constexpr int	min_exponent	= -15;
		static constexpr int	min_exponent10	= -1;	// TODO: check

		ND_ static constexpr AE::Math::Float16  min () noexcept { return AE::Math::Float16::Min(); }

		ND_ static constexpr AE::Math::Float16  max () noexcept { return AE::Math::Float16::Max(); }
	};
	
	template <>
	class numeric_limits< AE::Math::UFloat16 > final
	{
	public:
		static constexpr bool	is_signed		= false;
		static constexpr bool	is_specialized	= true;
		static constexpr int	radix			= 2;
		static constexpr int	digits			= 11;
		static constexpr int	digits10		= 3;	// TODO: check
		static constexpr int	max_digits10	= 1;	// TODO: check
		static constexpr int	max_exponent	= 16;
		static constexpr int	max_exponent10	= 1;	// TODO: check
		static constexpr int	min_exponent	= -15;
		static constexpr int	min_exponent10	= -1;	// TODO: check

		ND_ static constexpr AE::Math::UFloat16  min () noexcept { return AE::Math::UFloat16::Min(); }

		ND_ static constexpr AE::Math::UFloat16  max () noexcept { return AE::Math::UFloat16::Max(); }
	};

} // std
