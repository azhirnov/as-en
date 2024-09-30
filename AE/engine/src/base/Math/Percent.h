// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Algorithms/Cast.h"

namespace AE::Math
{

	//
	// Percent
	//

	template <typename T>
	struct TPercent
	{
	// types
	public:
		using Value_t	= T;
		using Self		= TPercent<T>;


	// variables
	private:
		T		_value;


	// methods
	public:
		constexpr TPercent ()										__NE___ = default;
		explicit constexpr TPercent (T val)							__NE___ : _value{val} {}

		NdCx__ bool  operator == (const Self rhs)					C_NE___	{ return _value == rhs._value; }
		NdCx__ bool  operator != (const Self rhs)					C_NE___	{ return _value != rhs._value; }
		NdCx__ bool  operator <  (const Self rhs)					C_NE___	{ return _value <  rhs._value; }
		NdCx__ bool  operator >  (const Self rhs)					C_NE___	{ return _value >  rhs._value; }
		NdCx__ bool  operator <= (const Self rhs)					C_NE___	{ return _value <= rhs._value; }
		NdCx__ bool  operator >= (const Self rhs)					C_NE___	{ return _value >= rhs._value; }

		NdCx__ T	GetPercent ()									C_NE___	{ return _value * T{100}; }				// 0..100%
		NdCx__ T	GetFraction ()									C_NE___	{ return _value; }						// 0..1

		template <typename B>
		NdCx__ EnableIf<IsFloatPoint<B>, B>  Of (const B &value)	C_NE___	{ return value * B{GetFraction()}; }

		template <typename B>
		NdCx__ static Self	FromPercent (B value)					__NE___	{ return Self{ T(value) * T{0.01} }; }
		NdCx__ static Self	FromFraction (T value)					__NE___	{ return Self{ value }; }
	};


	using Percent	= TPercent< float >;
	using PercentD	= TPercent< double >;

	NdCx__ Percent operator "" _pct (long double value)				__NE___ { return Percent{ Percent::FromPercent( Percent::Value_t( value ))}; }
	NdCx__ Percent operator "" _pct (unsigned long long value)		__NE___ { return Percent{ Percent::FromPercent( Percent::Value_t( value ))}; }


} // AE::Math


namespace AE::Base
{
	template <typename T>	struct TMemCopyAvailable< TPercent<T> >			: CT_Bool< IsMemCopyAvailable<T>		>{};
	template <typename T>	struct TZeroMemAvailable< TPercent<T> >			: CT_Bool< IsZeroMemAvailable<T>		>{};
	template <typename T>	struct TTriviallySerializable< TPercent<T> >	: CT_Bool< IsTriviallySerializable<T>	>{};

} // AE::Base

