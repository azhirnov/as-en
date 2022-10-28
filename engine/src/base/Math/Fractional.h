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
		constexpr Fractional () {}

		constexpr explicit Fractional (T num, T denom = T{1})
		{
			ASSERT( denom > 0 );
			const T	gcd = _GreatestCommonDivisor( num, denom );
			if ( gcd ) {
				numerator	= num / gcd;
				denominator	= denom / gcd;
			}
		}

		constexpr Fractional (const Self &) = default;
		
		constexpr Self&  operator = (const Self &) = default;


		ND_ constexpr Self  operator - () const
		{
			return Self{ -numerator, denominator };
		}

		ND_ constexpr Self  operator + (const Self &rhs) const
		{
			return Self{ numerator * rhs.denominator + rhs.numerator * denominator, denominator * rhs.denominator };
		}
		
		ND_ constexpr Self  operator - (const Self &rhs) const
		{
			return Self{ numerator * rhs.denominator - rhs.numerator * denominator, denominator * rhs.denominator };
		}
		
		ND_ constexpr Self  operator * (const Self &rhs) const
		{
			return Self{ numerator * rhs.numerator, denominator * rhs.denominator };
		}
		
		ND_ constexpr Self  operator / (const Self &rhs) const
		{
			return Self{ numerator * rhs.denominator, denominator * rhs.numerator };
		}

		ND_ constexpr Self  Pow (uint value) const
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

		ND_ constexpr bool  operator == (const Self &rhs) const
		{
			return	((numerator == T{0}) & (rhs.numerator == T{0}))				|
					((numerator == rhs.numerator) & (denominator == rhs.denominator));
		}

		ND_ constexpr bool  IsZero ()			const	{ return numerator == T{0}; }

		ND_ constexpr bool  IsInteger ()		const	{ return denominator == T{1}; }

		ND_ constexpr bool	IsPositive ()		const	{ return numerator >  T{0}; }
		ND_ constexpr bool	IsPositiveOrZero ()	const	{ return numerator >= T{0}; }
		ND_ constexpr bool	IsNegative ()		const	{ return numerator <  T{0}; }
		ND_ constexpr bool	IsNegativeOrZero ()	const	{ return numerator <= T{0}; }

		template <typename R>
		ND_ constexpr EnableIf<IsFloatPoint<R>, R>	ToFloat ()	const	{ return R(numerator) / R(denominator); }


		// Rounding:
		// RTS - round to smaller, similar to floor(val * float(frac))
		// RTN - round to nearest, similar to round(val * float(frac))
		// RTL - round to larger,  similar to ceil(val * float(frac))

		template <typename R>
		ND_ constexpr R  Mul_RTS (const R &val) const	{ return (val * numerator) / denominator; }
		
		template <typename R>
		ND_ constexpr R  Mul_RTN (const R &val) const	{ return ((val * numerator) + R(denominator/2)) / denominator; }

		template <typename R>
		ND_ constexpr R  Mul_RTL (const R &val) const	{ return ((val * numerator) + R(denominator-1)) / denominator; }


	private:
		static constexpr T  _GreatestCommonDivisor (T value1, T value2)
		{
			return value2 != 0 ? _GreatestCommonDivisor( value2, value1 % value2 ) : value1;
		}
	};
	

	using FractionalI	= Fractional< int >;
	using FractionalI16	= Fractional< sshort >;


} // AE::Math
