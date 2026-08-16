// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "base/Math/Vec.h"

namespace AE::Base
{

	//
	// Radians
	//

	template <typename T>
	struct TRadian
	{
		StaticAssert( IsScalar<T> and IsFloatPoint<T> );

	// types
	public:
		using Self		= TRadian<T>;
		using Value_t	= T;


	// variables
	private:
		T		_value;


	// methods
	public:
		__Cx__ TRadian ()									__NE___	: _value{} {}
		__Cx__ explicit TRadian (T val)						__NE___	: _value{val} {}

		template <typename B>
		__Cx__ explicit TRadian (TRadian<B> other)			__NE___	: _value{T(other.Ref())} {}

		__Cx__ TRadian (const Self &)						__NE___	= default;
		__Cx__ TRadian (Self &&)							__NE___	= default;

		template <typename B=T> requires( IsSame< B, float >)
		NdCx__ explicit operator double ()					C_NE___	{ return double(_value); }
		NdCx__ explicit operator T ()						C_NE___	{ return _value; }

		__Cx__ Self&	operator = (const Self &)			__NE___	= default;
		__Cx__ Self&	operator = (Self &&)				__NE___	= default;

		NdCx__ Self		operator - ()						C_NE___	{ return Self{ -_value }; }

		NdCx__ Self		WrapToPi ()							C_NE___	{ return Self{Wrap( _value, -Pi()._value, Pi()._value  )}; }
		NdCx__ Self		WrapTo0_2Pi ()						C_NE___	{ return Self{Wrap( _value, T{0}, Pi2()._value  )}; }

		NdCx__ T		ToDeg ()							C_NE___	{ return _value * RadToDeg(); }
		NdCx__ T&		Ref ()								__NE___	{ return _value; }

		__Cx__ Self&	operator += (const Self rhs)		__NE___	{ _value += rhs._value;  return *this; }
		__Cx__ Self&	operator -= (const Self rhs)		__NE___	{ _value -= rhs._value;  return *this; }
		__Cx__ Self&	operator *= (const Self rhs)		__NE___	{ _value *= rhs._value;  return *this; }
		__Cx__ Self&	operator /= (const Self rhs)		__NE___	{ _value /= rhs._value;  return *this; }

		__Cx__ Self&	operator += (const T rhs)			__NE___	{ _value += rhs;  return *this; }
		__Cx__ Self&	operator -= (const T rhs)			__NE___	{ _value -= rhs;  return *this; }
		__Cx__ Self&	operator *= (const T rhs)			__NE___	{ _value *= rhs;  return *this; }
		__Cx__ Self&	operator /= (const T rhs)			__NE___	{ _value /= rhs;  return *this; }

		NdCx__ Self		operator + (const Self rhs)			C_NE___	{ return Self{ _value + rhs._value }; }
		NdCx__ Self		operator - (const Self rhs)			C_NE___	{ return Self{ _value - rhs._value }; }
		NdCx__ Self		operator * (const Self rhs)			C_NE___	{ return Self{ _value * rhs._value }; }
		NdCx__ Self		operator / (const Self rhs)			C_NE___	{ return Self{ _value / rhs._value }; }

		NdCx__ Self		operator + (const T rhs)			C_NE___	{ return Self{ _value + rhs }; }
		NdCx__ Self		operator - (const T rhs)			C_NE___	{ return Self{ _value - rhs }; }
		NdCx__ Self		operator * (const T rhs)			C_NE___	{ return Self{ _value * rhs }; }
		NdCx__ Self		operator / (const T rhs)			C_NE___	{ return Self{ _value / rhs }; }

		NdCx__ bool		operator == (const Self rhs)		C_NE___	{ return _value == rhs._value; }
		NdCx__ bool		operator != (const Self rhs)		C_NE___	{ return _value != rhs._value; }
		NdCx__ bool		operator >  (const Self rhs)		C_NE___	{ return _value >  rhs._value; }
		NdCx__ bool		operator <  (const Self rhs)		C_NE___	{ return _value <  rhs._value; }
		NdCx__ bool		operator >= (const Self rhs)		C_NE___	{ return _value >= rhs._value; }
		NdCx__ bool		operator <= (const Self rhs)		C_NE___	{ return _value <= rhs._value; }

		NdCx__ friend Self  operator + (T lhs, Self rhs)	__NE___	{ return Self{ lhs + rhs._value }; }
		NdCx__ friend Self  operator - (T lhs, Self rhs)	__NE___	{ return Self{ lhs - rhs._value }; }
		NdCx__ friend Self  operator * (T lhs, Self rhs)	__NE___	{ return Self{ lhs * rhs._value }; }
		NdCx__ friend Self  operator / (T lhs, Self rhs)	__NE___	{ return Self{ lhs / rhs._value }; }


		NdCx__ static Self	Pi ()							__NE___	{ return Self{T( 3.1415926535897932384626433832795 )}; }
		NdCx__ static Self	Pi2 ()							__NE___	{ return Self{T( 6.283185307179586476925286766559  )}; }
		NdCx__ static Self	HalfPi ()						__NE___	{ return Self{T( 1.5707963267948966192313216916398 )}; }
		NdCx__ static T		ReciprocalPi ()					__NE___	{ return T( 0.31830988618379067153776752674503 ); }

		NdCx__ static T		DegToRad ()						__NE___	{ return T( 0.01745329251994329576923690768489 ); }
		NdCx__ static T		RadToDeg ()						__NE___	{ return T( 57.295779513082320876798154814105 ); }

		NdCx__ static Self	FromDeg (T value)				__NE___	{ return Self{ DegToRad() * value }; }
	};


	template <typename T, int I>
	using RadianVec	= Vec< TRadian<T>, I >;

	using Rad		= TRadian<float>;
	using Rad2		= RadianVec< float, 2 >;
	using Rad3		= RadianVec< float, 3 >;

	inline static constexpr Rad  Pi = Rad::Pi();

	NdCx__ Rad  operator ""_rad (long double value)			__NE___	{ return Rad{ Rad::Value_t(value) }; }
	NdCx__ Rad  operator ""_rad (unsigned long long value)	__NE___	{ return Rad{ Rad::Value_t(value) }; }

	NdCx__ Rad  operator ""_deg (long double value)			__NE___	{ return Rad::FromDeg( Rad::Value_t(value) ); }
	NdCx__ Rad  operator ""_deg (unsigned long long value)	__NE___	{ return Rad::FromDeg( Rad::Value_t(value) ); }


/*
=================================================
	Abs
=================================================
*/
	template <typename T>
	ND_ TRadian<T>  Abs (const TRadian<T>& x) __NE___
	{
		return TRadian<T>{ std::abs( T(x) )};
	}

/*
=================================================
	Sin
=================================================
*/
	template <typename T>
	ND_ T  Sin (const TRadian<T>& x) __NE___
	{
		return std::sin( T(x) );
	}

/*
=================================================
	Cos
=================================================
*/
	template <typename T>
	ND_ T  Cos (const TRadian<T>& x) __NE___
	{
		return std::cos( T(x) );
	}

/*
=================================================
	SinCos
=================================================
*/
	template <typename T>
	ND_ Vec<T,2>  SinCos (const TRadian<T>& x) __NE___
	{
		return Vec<T,2>{ std::sin( T(x) ), std::cos( T(x) )};
	}

/*
=================================================
	ASin
----
	result in range [-Pi/2 ... Pi/2],
	result is undefined if abs(x) > 1
----
	for right-angled triangle angle it is opposite catheter div by hypotenuse
=================================================
*/
	template <typename T>
	ND_ TRadian<T>  ASin (const T& x) __NE___
	{
		ASSERT( x >= T{-1} and x <= T{1} );

		return TRadian<T>{std::asin( x )};
	}

/*
=================================================
	ACos
----
	result in range [0 .. Pi],
	result is undefined if abs(x) > 1
----
	for right-angled triangle angle it is adjacent catheter div by hypotenuse
=================================================
*/
	template <typename T>
	ND_ TRadian<T>  ACos (const T& x) __NE___
	{
		ASSERT( x >= T{-1} and x <= T{1} );

		return TRadian<T>{std::acos( x )};
	}

/*
=================================================
	SinH
=================================================
*/
	template <typename T>
	ND_ T  SinH (const TRadian<T>& x) __NE___
	{
		return std::sinh( T(x) );
	}

/*
=================================================
	CosH
=================================================
*/
	template <typename T>
	ND_ T  CosH (const TRadian<T>& x) __NE___
	{
		return std::cosh( T(x) );
	}

/*
=================================================
	ASinH
=================================================
*/
	template <typename T>
	ND_ TRadian<T>  ASinH (const T& x) __NE___
	{
		return TRadian<T>( SignOrZero( x ) * Ln( x + Sqrt( (x*x) + T{1} )));
	}

/*
=================================================
	ACosH
=================================================
*/
	template <typename T>
	ND_ TRadian<T>  ACosH (const T& x) __NE___
	{
		ASSERT( x >= T{1} );
		return TRadian<T>{ Ln( x + Sqrt( (x*x) - T{1} ))};
	}

/*
=================================================
	Tan
=================================================
*/
	template <typename T>
	ND_ T  Tan (const TRadian<T>& x) __NE___
	{
		return std::tan( T(x) );
	}

/*
=================================================
	CoTan
=================================================
*/
	template <typename T>
	ND_ T  CoTan (const TRadian<T>& x) __NE___
	{
		return SafeDiv( T{1}, Tan( x ), T{0} );
	}

/*
=================================================
	TanH
=================================================
*/
	template <typename T>
	ND_ T  TanH (const TRadian<T>& x) __NE___
	{
		return std::tanh( T(x) );
	}

/*
=================================================
	CoTanH
=================================================
*/
	template <typename T>
	ND_ T  CoTanH (const TRadian<T>& x) __NE___
	{
		return SafeDiv( T{1}, TanH( x ), T{0} );
	}

/*
=================================================
	ATan
----
	result in range [-Pi/2 ... Pi/2], result is undefined if y_over_x == 0
----
	for right-angled triangle angle: opposite catheter div by adjacent catheter

	    /|
	  /  | y	a = ATan( y / x )
  a /____|
	  x
=================================================
*/
	template <typename T>
	ND_ TRadian<T>  ATan (const T& y_over_x) __NE___
	{
		return TRadian<T>{ std::atan( y_over_x )};
	}

/*
=================================================
	ATan
----
	result in range [-Pi...+Pi]
=================================================
*/
	template <typename T>
	ND_ TRadian<T>  ATan (const T& y, const T& x) __NE___
	{
		return TRadian<T>{ std::atan2( y, x )};
	}

/*
=================================================
	ACoTan
=================================================
*/
	template <typename T>
	ND_ TRadian<T>  ACoTan (const T& x) __NE___
	{
		return TRadian<T>{ SafeDiv( T{1}, ATan( x ), T{0} )};
	}

/*
=================================================
	ATanH
=================================================
*
	template <typename T>
	ND_ Radians<T>  ATanH (const T& x) __NE___
	{
		ASSERT( x > T{-1} and x < T{1} );

		if ( Abs(x) == T{1} )	return Infinity<T>();	else
		if ( Abs(x) > T{1} )	return NaN<T>();		else
								return Ln( (T{1} + x) / (T{1} - x) ) / T(2);
	}

/*
=================================================
	ACoTanH
=================================================
*/
	template <typename T>
	ND_ TRadian<T>  ACoTanH (const T& x) __NE___
	{
		return TRadian<T>{ SafeDiv( T{1}, ATanH( x ), T{0} )};
	}

/*
=================================================
	Equal
=================================================
*/
	template <typename T>
	NdCx__ bool  Equal (const TRadian<T> &lhs, const TRadian<T> &rhs, const T err = Epsilon<T>()) __NE___
	{
		return Equal( T{lhs}, T{rhs}, err );
	}

	template <typename T>
	NdCx__ bool  Equal (const TRadian<T> &lhs, const TRadian<T> &rhs, const Percent err) __NE___
	{
		return Equal( T{lhs}, T{rhs}, err );
	}

/*
=================================================
	BitEqual
=================================================
*/
	template <typename T> requires(IsFloatPoint<T>)
	NdCx__ bool  BitEqual (const TRadian<T> &lhs, const TRadian<T> &rhs, const EnabledBitCount bitCount) __NE___
	{
		return BitEqual( T{lhs}, T{rhs}, bitCount );
	}

	template <typename T> requires(IsFloatPoint<T>)
	NdCx__ bool  BitEqual (const TRadian<T> &lhs, const TRadian<T> &rhs) __NE___
	{
		return BitEqual( T{lhs}, T{rhs} );
	}

/*
=================================================
	IsZero / IsNotZero
=================================================
*/
	template <typename T> requires(IsScalar<T>)
	NdCx__ bool  IsZero (const TRadian<T> &x) __NE___
	{
		return IsZero( T{x} );
	}

	template <typename T> requires(IsScalar<T>)
	NdCx__ bool  IsNotZero (const TRadian<T> &x) __NE___
	{
		return IsNotZero( T{x} );
	}

/*
=================================================
	Lerp
=================================================
*/
	template <typename T, typename B>
	NdCx__ TRadian<T>  Lerp (const TRadian<T> &x, const TRadian<T> &y, const B& factor) __NE___
	{
		return TRadian<T>{ Base::Lerp( T{x}, T{y}, factor )};
	}
//-----------------------------------------------------------------------------


	template <typename T>	struct TMemCopyAvailable< TRadian<T> >		: CT_Bool< IsMemCopyAvailable<T>		>{};
	template <typename T>	struct TZeroMemAvailable< TRadian<T> >		: CT_Bool< IsZeroMemAvailable<T>		>{};
	template <typename T>	struct TTriviallySerializable< TRadian<T> >	: CT_Bool< IsTriviallySerializable<T>	>{};

	template <typename T>	struct TIsScalar< TRadian<T> >		: CT_True {};
	template <typename T>	struct TIsSigned< TRadian<T> >		: TIsSigned<T> {};
	template <typename T>	struct TIsUnsigned< TRadian<T> >	: TIsUnsigned<T> {};
	template <typename T>	struct TIsFloatPoint< TRadian<T> >	: TIsFloatPoint<T> {};
	template <typename T>	struct TUnwrap< TRadian<T> >		: TUnwrap<T> {};

} // AE::Base
