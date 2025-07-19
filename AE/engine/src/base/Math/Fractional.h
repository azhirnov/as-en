// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Common.h"

namespace AE::Base
{

	//
	// Fractional
	//

	template <typename T>
	struct Fractional
	{
		StaticAssert( IsInteger<T> );

	// types
	public:
		using Self	= Fractional<T>;


	// variables
	public:
		T	num	{0};		// numerator
		T	den	{1};		// denominator


	// methods
	public:
		__Cx__ Fractional ()							__NE___ {}

		__Cx__ explicit Fractional (T inNum, T inDenom = T{1})	__NE___
		{
			ASSERT_Cx( inDenom > 0 );
			const T	gcd = _GreatestCommonDivisor( inNum, inDenom );
			if ( gcd != 0 ) {
				num	= inNum / gcd;
				den	= inDenom / gcd;
			}
		}

		__Cx__ Fractional (const Self &)				__NE___ = default;

		__Cx__ Self&  operator = (const Self &)			__NE___ = default;


		NdCx__ Self  operator - ()						C_NE___
		{
			return Self{ -num, den };
		}

		NdCx__ Self  operator + (const Self &rhs)		C_NE___
		{
			return Self{ this->num * rhs.den + rhs.num * den, this->den * rhs.den };
		}

		NdCx__ Self  operator - (const Self &rhs)		C_NE___
		{
			return Self{ this->num * rhs.den - rhs.num * den, this->den * rhs.den };
		}

		NdCx__ Self  operator * (const Self &rhs)		C_NE___
		{
			return Self{ this->num * rhs.num, this->den * rhs.den };
		}

		NdCx__ Self  operator / (const Self &rhs)		C_NE___
		{
			return Self{ this->num * rhs.den, this->den * rhs.num };
		}

		NdCx__ Self  Pow (uint value)					C_NE___
		{
			Self	result;
			result.num	= this->num;
			result.den	= this->den;

			for (uint i = 0; i < value; ++i) {
				result.num	*= this->num;
				result.den	*= this->den;
			}
			return result;
		}

		NdCx__ bool  operator == (const Self &rhs)		C_NE___
		{
			return	((this->num == T{0})    and (rhs.num == T{0}))	or
					((this->num == rhs.num) and (this->den == rhs.den));
		}

		NdCx__ bool  IsZero ()							C_NE___	{ return num == T{0}; }

		NdCx__ bool  IsInteger ()						C_NE___	{ return den == T{1}; }

		NdCx__ bool	IsPositive ()						C_NE___	{ return num >  T{0}; }
		NdCx__ bool	IsPositiveOrZero ()					C_NE___	{ return num >= T{0}; }
		NdCx__ bool	IsNegative ()						C_NE___	{ return num <  T{0}; }
		NdCx__ bool	IsNegativeOrZero ()					C_NE___	{ return num <= T{0}; }

		NdCx__ T	Numerator ()						C_NE___	{ return num; }
		NdCx__ T	Denominator ()						C_NE___	{ return den; }

		template <typename R> requires(IsFloatPoint<R>)
		NdCx__ R	ToFloat ()							C_NE___	{ return R(num) / R(den); }


		// Rounding:
		// RTS - round to smaller, similar to floor(val * float(fract))
		// RTN - round to nearest, similar to round(val * float(fract))
		// RTL - round to larger,  similar to ceil(val * float(fract))

		template <typename R>
		NdCx__ R  Get_RTS ()							C_NE___	{ return R(num) / den; }
		NdCx__ T  Get_RTS ()							C_NE___	{ return Get_RTS<T>(); }

		template <typename R>
		NdCx__ R  Get_RTN ()							C_NE___	{ return (num + R(den/2)) / den; }
		NdCx__ T  Get_RTN ()							C_NE___	{ return Get_RTN<T>(); }

		template <typename R>
		NdCx__ R  Get_RTL ()							C_NE___	{ return (num + R(den-1)) / den; }
		NdCx__ T  Get_RTL ()							C_NE___	{ return Get_RTL<T>(); }


		template <typename R>
		NdCx__ R  Mul_RTS (const R &val)				C_NE___	{ return (val * num) / den; }

		template <typename R>
		NdCx__ R  Mul_RTN (const R &val)				C_NE___	{ return ((val * num) + R(den/2)) / den; }

		template <typename R>
		NdCx__ R  Mul_RTL (const R &val)				C_NE___	{ return ((val * num) + R(den-1)) / den; }


		template <typename R>
		NdCx__ R  Div_RTS (const R &val)				C_NE___	{ ASSERT( num != 0 );  return (val * den) / num; }

		template <typename R>
		NdCx__ R  Div_RTN (const R &val)				C_NE___	{ ASSERT( num != 0 );  return ((val * den) + R(num/2)) / num; }

		template <typename R>
		NdCx__ R  Div_RTL (const R &val)				C_NE___	{ ASSERT( num != 0 );  return ((val * den) + R(num-1)) / num; }


	private:
		NdCx__ static T  _GreatestCommonDivisor (T value1, T value2) __NE___
		{
			return value2 != 0 ? _GreatestCommonDivisor( value2, value1 % value2 ) : value1;
		}
	};


	using FractionalI	= Fractional< int >;
	using FractionalU	= Fractional< uint >;

	using FractionalI16	= Fractional< sshort >;
	using FractionalU16	= Fractional< ushort >;
//-----------------------------------------------------------------------------


	template <typename T>	struct TMemCopyAvailable< Fractional<T> >		: CT_Bool< IsMemCopyAvailable<T>		>{};
	template <typename T>	struct TZeroMemAvailable< Fractional<T> >		: CT_Bool< IsZeroMemAvailable<T>		>{};
	template <typename T>	struct TTriviallySerializable< Fractional<T> >	: CT_Bool< IsTriviallySerializable<T>	>{};
	template <typename T>	struct TIsScalar< Fractional<T> >				: CT_True {};
	template <typename T>	struct TIsSigned< Fractional<T> >				: TIsSigned<T> {};
	template <typename T>	struct TIsUnsigned< Fractional<T> >				: TIsUnsigned<T> {};
	template <typename T>	struct TUnwrap< Fractional<T> >					: TUnwrap<T> {};

} // AE::Base

