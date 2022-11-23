// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"

namespace AE::Math
{

	//
	// Fractional
	//

	template <typename T>
	struct Fractional
	{
	// types
		using Self	= Fractional<T>;


	// variables
		T	numerator	{0};
		T	denominator	{1};


	// methods
		constexpr Fractional ()									__NE___ {}

		constexpr explicit Fractional (T num, T denom = T{1})	__NE___
		{
			ASSERT( denom > 0 );
			const T	gcd = _GreatestCommonDivisor( num, denom );
			if ( gcd ) {
				numerator	= num / gcd;
				denominator	= denom / gcd;
			}
		}

		constexpr Fractional (const Self &)						__NE___ = default;
		
		constexpr Self&  operator = (const Self &)				__NE___ = default;


		ND_ constexpr Self  operator - ()						C_NE___
		{
			return Self{ -numerator, denominator };
		}

		ND_ constexpr Self  operator + (const Self &rhs)		C_NE___
		{
			return Self{ numerator * rhs.denominator + rhs.numerator * denominator, denominator * rhs.denominator };
		}
		
		ND_ constexpr Self  operator - (const Self &rhs)		C_NE___
		{
			return Self{ numerator * rhs.denominator - rhs.numerator * denominator, denominator * rhs.denominator };
		}
		
		ND_ constexpr Self  operator * (const Self &rhs)		C_NE___
		{
			return Self{ numerator * rhs.numerator, denominator * rhs.denominator };
		}
		
		ND_ constexpr Self  operator / (const Self &rhs)		C_NE___
		{
			return Self{ numerator * rhs.denominator, denominator * rhs.numerator };
		}

		ND_ constexpr Self  Pow (uint value)					C_NE___
		{
			Self	result;
			result.numerator	= numerator;
			result.denominator	= denominator;

			for (uint i = 0; i < value; ++i) {
				result.numerator	*= numerator;
				result.denominator	*= denominator;
			}
			return result;
		}

		ND_ constexpr bool  operator == (const Self &rhs)		C_NE___
		{
			return	((numerator == T{0}) & (rhs.numerator == T{0}))				|
					((numerator == rhs.numerator) & (denominator == rhs.denominator));
		}

		ND_ constexpr bool  IsZero ()							C_NE___	{ return numerator == T{0}; }

		ND_ constexpr bool  IsInteger ()						C_NE___	{ return denominator == T{1}; }

		ND_ constexpr bool	IsPositive ()						C_NE___	{ return numerator >  T{0}; }
		ND_ constexpr bool	IsPositiveOrZero ()					C_NE___	{ return numerator >= T{0}; }
		ND_ constexpr bool	IsNegative ()						C_NE___	{ return numerator <  T{0}; }
		ND_ constexpr bool	IsNegativeOrZero ()					C_NE___	{ return numerator <= T{0}; }

		template <typename R>
		ND_ constexpr EnableIf<IsFloatPoint<R>, R>	ToFloat ()	C_NE___	{ return R(numerator) / R(denominator); }


		// Rounding:
		// RTS - round to smaller, similar to floor(val * float(frac))
		// RTN - round to nearest, similar to round(val * float(frac))
		// RTL - round to larger,  similar to ceil(val * float(frac))

		template <typename R>
		ND_ constexpr R  Mul_RTS (const R &val)					C_NE___	{ return (val * numerator) / denominator; }
		
		template <typename R>
		ND_ constexpr R  Mul_RTN (const R &val)					C_NE___	{ return ((val * numerator) + R(denominator/2)) / denominator; }

		template <typename R>
		ND_ constexpr R  Mul_RTL (const R &val)					C_NE___	{ return ((val * numerator) + R(denominator-1)) / denominator; }


	private:
		static constexpr T  _GreatestCommonDivisor (T value1, T value2) __NE___
		{
			return value2 != 0 ? _GreatestCommonDivisor( value2, value1 % value2 ) : value1;
		}
	};
	

	using FractionalI	= Fractional< int >;
	using FractionalI16	= Fractional< sshort >;


} // AE::Math
