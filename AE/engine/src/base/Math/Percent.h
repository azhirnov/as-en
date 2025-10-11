// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Algorithms/Cast.h"

namespace AE::Base
{

	//
	// Percent
	//

	template <typename T>
	struct TPercent
	{
		StaticAssert( IsAnyFloatPoint<T> );

	// types
	public:
		using Value_t	= T;
		using Self		= TPercent<T>;


	// variables
	private:
		T		_value;


	// methods
	public:
		__Cx__ TPercent ()											__NE___ = default;
		__Cx__ explicit TPercent (T val)							__NE___ : _value{val} {}

		NdCx__ bool  operator == (const Self rhs)					C_NE___	{ return _value == rhs._value; }
		NdCx__ bool  operator != (const Self rhs)					C_NE___	{ return _value != rhs._value; }
		NdCx__ bool  operator <  (const Self rhs)					C_NE___	{ return _value <  rhs._value; }
		NdCx__ bool  operator >  (const Self rhs)					C_NE___	{ return _value >  rhs._value; }
		NdCx__ bool  operator <= (const Self rhs)					C_NE___	{ return _value <= rhs._value; }
		NdCx__ bool  operator >= (const Self rhs)					C_NE___	{ return _value >= rhs._value; }

		NdCx__ T	GetPercent ()									C_NE___	{ return _value * T(100.0); }			// 0..100%
		NdCx__ T	GetFraction ()									C_NE___	{ return _value; }						// 0..1

		template <typename B> requires(IsFloatPoint<B>)
		NdCx__ B	Of (const B &value)								C_NE___	{ return value * B{GetFraction()}; }

		template <typename B>
		NdCx__ static Self	FromPercent (B value)					__NE___	{ return Self{ T(value) * T(0.01) }; }
		NdCx__ static Self	FromFraction (T value)					__NE___	{ return Self{ value }; }
	};


	using Percent	= TPercent< float >;
	using PercentD	= TPercent< double >;

	NdCx__ Percent operator ""_pct (long double value)				__NE___ { return Percent{ Percent::FromPercent( Percent::Value_t( value ))}; }
	NdCx__ Percent operator ""_pct (unsigned long long value)		__NE___ { return Percent{ Percent::FromPercent( Percent::Value_t( value ))}; }
//-----------------------------------------------------------------------------


	template <typename T>	struct TMemCopyAvailable< TPercent<T> >			: CT_Bool< IsMemCopyAvailable<T>		>{};
	template <typename T>	struct TZeroMemAvailable< TPercent<T> >			: CT_Bool< IsZeroMemAvailable<T>		>{};
	template <typename T>	struct TTriviallySerializable< TPercent<T> >	: CT_Bool< IsTriviallySerializable<T>	>{};

	template <typename T>	struct TIsScalar< TPercent<T> >		: CT_True {};
	template <typename T>	struct TIsSigned< TPercent<T> >		: TIsSigned<T> {};
	template <typename T>	struct TIsUnsigned< TPercent<T> >	: TIsUnsigned<T> {};
	template <typename T>	struct TIsFloatPoint< TPercent<T> >	: TIsFloatPoint<T> {};
	template <typename T>	struct TUnwrap< TPercent<T> >		: TUnwrap<T> {};

} // AE::Base

