// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/Math/Bytes.h"

namespace AE::Math
{

/*
=================================================
	helpers
=================================================
*/
	namespace _hidden_
	{
		template <typename T1, typename T2, typename Result>
		using EnableForInt		= EnableIf< IsSignedInteger<T1> and IsSignedInteger<T2>, Result >;
		
		template <typename T1, typename T2, typename Result>
		using EnableForUInt		= EnableIf< IsUnsignedInteger<T1> and IsUnsignedInteger<T2>, Result >;

	} // _hidden_
	
/*
=================================================
	AdditionIsSafe
=================================================
*/
	template <typename T1, typename T2>
	ND_ constexpr Math::_hidden_::EnableForInt<T1, T2, bool>  AdditionIsSafe (const T1 a, const T2 b) __NE___
	{
		STATIC_ASSERT( IsScalar<T1> and IsScalar<T2> );

		using T = decltype(a + b);

		const T	x	= T(a);
		const T	y	= T(b);
		const T	min = MinValue<T>();
		const T	max = MaxValue<T>();
		
		bool	overflow =	((y > 0) & (x > max - y))	|
							((y < 0) & (x < min - y));
		return not overflow;
	}

/*
=================================================
	AdditionIsSafe
=================================================
*/
	template <typename T1, typename T2>
	ND_ constexpr Math::_hidden_::EnableForUInt<T1, T2, bool>  AdditionIsSafe (const T1 a, const T2 b) __NE___
	{
		STATIC_ASSERT( IsScalar<T1> and IsScalar<T2> );
		
		using T = decltype(a + b);
		
		const T	x	= T(a);
		const T	y	= T(b);

		return (x + y) >= (x | y);
	}

/*
=================================================
	CalcAverage
=================================================
*/
	template <typename T>
	ND_ constexpr T  CalcAverage (T begin, T end) __NE___
	{
		if constexpr( IsFloatPoint<T> )
			return (begin * T{0.5}) + (end * T{0.5});
		else
		if constexpr( IsUnsignedInteger<T> )
			return (begin >> 1) + (end >> 1) + (((begin & 1) + (end & 1)) >> 1);
		else
		if constexpr( IsSignedInteger<T> )
			return (begin / 2) + (end / 2) + ((begin % 2) + (end % 2)) / 2;
	}
	
	template <typename T>
	ND_ constexpr TBytes<T>  CalcAverage (TBytes<T> begin, TBytes<T> end) __NE___
	{
		return TBytes<T>{ CalcAverage( T{begin}, T{end} )};
	}

/*
=================================================
	AlignDown
=================================================
*/
	template <typename T0, typename T1>
	ND_ constexpr auto  AlignDown (const T0 &value, const T1 &align) __NE___
	{
		ASSERT( align > 0 );
		if constexpr( IsPointer<T0> )
		{
			Bytes	byte_align{ align };
			return BitCast<T0>( (BitCast<usize>(value) / byte_align) * byte_align );
		}
		else
			return (value / align) * align;
	}

/*
=================================================
	AlignUp
=================================================
*/
	template <typename T0, typename T1>
	ND_ constexpr auto  AlignUp (const T0 &value, const T1 &align) __NE___
	{
		ASSERT( align > 0 );
		if constexpr( IsPointer<T0> )
		{
			Bytes	byte_align{ align };
			return BitCast<T0>( ((BitCast<usize>(value) + byte_align-1) / byte_align) * byte_align );
		}
		else
			return ((value + align-1) / align) * align;
	}
	
/*
=================================================
	IsAligned
=================================================
*/
	template <typename T0, typename T1>
	ND_ constexpr bool  IsAligned (const T0 &value, const T1 &align) __NE___
	{
		ASSERT( align > 0 );
		if constexpr( IsPointer<T0> )
		{
			return BitCast<usize>(value) % Bytes{align} == 0;
		}
		else
			return value % align == 0;
	}

/*
=================================================
	All/Any
=================================================
*/
	ND_ forceinline constexpr bool  All (const bool &value) __NE___
	{
		return value;
	}
	
	ND_ forceinline constexpr bool  Any (const bool &value) __NE___
	{
		return value;
	}
	
/*
=================================================
	Square
=================================================
*/
	template <typename T>
	ND_ constexpr T  Square (const T &value) __NE___
	{
		return value * value;
	}
	
/*
=================================================
	Sqrt
=================================================
*/
	template <typename T>
	ND_ constexpr EnableIf<IsFloatPoint<T>, T>  Sqrt (const T &value) __NE___
	{
		return std::sqrt( value );
	}

/*
=================================================
	Abs
=================================================
*/
	template <typename T>
	ND_ constexpr EnableIf<IsScalar<T>, T>  Abs (const T &x) __NE___
	{
		return std::abs( x );
	}
	
/*
=================================================
	Epsilon
=================================================
*/
	template <typename T>
	ND_ constexpr  EnableIf<IsScalar<T>, T>  Epsilon () __NE___
	{
		return std::numeric_limits<T>::epsilon() * T(2);
	}

/*
=================================================
	Equals
=================================================
*/
	template <typename T>
	ND_ constexpr EnableIf<IsScalar<T>, bool>  Equals (const T &lhs, const T &rhs, const T &err = Epsilon<T>()) __NE___
	{
		if constexpr( IsUnsignedInteger<T> )
		{
			return lhs < rhs ? ((rhs - lhs) <= err) : ((lhs - rhs) <= err);
		}else
			return Abs(lhs - rhs) <= err;
	}

/*
=================================================
	Equals
=================================================
*/
	template <typename T>
	ND_ bool  Equals (const Optional<T> &lhs, const Optional<T> &rhs) __NE___
	{
		return	lhs.has_value() == rhs.has_value()	and
				(lhs.has_value() ? *lhs == *rhs : false);
	}
	
/*
=================================================
	IsZero / IsNotZero
=================================================
*/
	template <typename T>
	ND_ constexpr EnableIf<IsScalar<T>, bool>  IsZero (const T &x) __NE___
	{
		return Equals( x, T{0}, Epsilon<T>() );
	}
	
	template <typename T>
	ND_ constexpr EnableIf<IsScalar<T>, bool>  IsNotZero (const T &x) __NE___
	{
		return not IsZero( x );
	}

/*
=================================================
	Floor / Ceil / Trunc
=================================================
*/
	template <typename T>
	ND_ EnableIf<IsScalar<T> and IsFloatPoint<T>, T>  Floor (const T& x) __NE___
	{
		return std::floor( x );
	}
	
	template <typename T>
	ND_ EnableIf<IsScalar<T> and IsFloatPoint<T>, T>  Ceil (const T& x) __NE___
	{
		return std::ceil( x );
	}
	
	template <typename T>
	ND_ EnableIf<IsScalar<T> and IsFloatPoint<T>, T>  Trunc (const T& x) __NE___
	{
#	if 1
		return std::trunc( x );
#	else
		return x > T{0} ? Floor(x) : Ceil(x);
#	endif
	}
	
/*
=================================================
	Round / RoundToInt / RoundToUint
=================================================
*/
	template <typename T>
	ND_ EnableIf<IsScalar<T> and IsFloatPoint<T>, T>  Round (const T& x) __NE___
	{
		return std::round( x );
	}

	template <typename T>
	ND_ auto  RoundToInt (const T& x) __NE___
	{
		STATIC_ASSERT( IsFloatPoint<T> );
		
		if constexpr( sizeof(T) >= sizeof(slong) )
			return slong(std::round( x ));

		if constexpr( sizeof(T) >= sizeof(int32_t) )
			return int32_t(std::round( x ));
	}

	template <typename T>
	ND_ auto  RoundToUint (const T& x) __NE___
	{
		STATIC_ASSERT( IsFloatPoint<T> );
		
		if constexpr( sizeof(T) >= sizeof(ulong) )
			return ulong(std::round( x ));

		if constexpr( sizeof(T) >= sizeof(uint) )
			return uint(std::round( x ));
	}

/*
=================================================
	Fract
----
	GLSL-style fract which returns value in range [0; 1)
=================================================
*/
	template <typename T>
	ND_ T  Fract (const T& x) __NE___
	{
		return x - Floor( x );
	}

/*
=================================================
	IsIntersects
----
	1D intersection check
=================================================
*/
	template <typename T>
	ND_ constexpr bool  IsIntersects (const T& begin1, const T& end1,
									  const T& begin2, const T& end2) __NE___
	{
		return (end1 > begin2) & (begin1 < end2);
	}

/*
=================================================
	IsCompletelyInside
=================================================
*/
	template <typename T>
	ND_ constexpr bool  IsCompletelyInside (const T& largeBlockBegin, const T& largeBlockEnd,
											const T& smallBlockBegin, const T& smallBlockEnd) __NE___
	{
		return (smallBlockBegin >= largeBlockBegin) & (smallBlockEnd <= largeBlockEnd);
	}

/*
=================================================
	GetIntersection
=================================================
*/
	template <typename T>
	ND_ constexpr bool  GetIntersection (const T& begin1, const T& end1,
										 const T& begin2, const T& end2,
										 OUT T& outBegin, OUT T& outEnd) __NE___
	{
		outBegin = Max( begin1, begin2 );
		outEnd   = Min( end1, end2 );
		return outBegin < outEnd;
	}

/*
=================================================
	Lerp
----
	linear interpolation
=================================================
*/
	template <typename T, typename B>
	ND_ constexpr EnableIf<IsScalar<T>, T>  Lerp (const T& x, const T& y, const B& factor) __NE___
	{
		//return T(factor) * (y - x) + x;
		return x * (T{1} - T(factor)) + y * T(factor);
	}

/*
=================================================
	Ln / Log / Log2 / Log10
=================================================
*/
	template <typename T>
	ND_ EnableIf<IsFloatPoint<T>, T>  Ln (const T& x) __NE___
	{
		ASSERT( x >= T{0} );
		return std::log( x );
	}

	template <typename T>
	ND_ EnableIf<IsFloatPoint<T>, T>  Log2 (const T& x) __NE___
	{
		ASSERT( x >= T{0} );
		return std::log2( x );
	}

	template <typename T>
	ND_ EnableIf<IsFloatPoint<T>, T>  Log10 (const T& x) __NE___
	{
		ASSERT( x >= T{0} );
		return std::log10( x );
	}

	template <auto Base, typename T>
	ND_ EnableIf<IsFloatPoint<T>, T>  Log (const T& x) __NE___
	{
		static constexpr auto log_base = std::log( Base );
		return Ln( x ) / log_base;
	}

	template <typename T>
	ND_ EnableIf<IsFloatPoint<T>, T>  Log (const T& x, const T& base) __NE___
	{
		return Ln( x ) / Ln( base );
	}
	
/*
=================================================
	Pow / Exp / Exp2 / Exp10 / ExpMinus1
=================================================
*/
	template <typename T>
	ND_ EnableIf<IsFloatPoint<T>, T>  Pow (const T& base, const T& power) __NE___
	{
		ASSERT( base >= T{0} or power == Floor(power) );	// if 'base' < 0 and 'power' not integer then result is NaN
		return std::pow( base, power );
	}

	template <typename T>
	ND_ EnableIf<IsFloatPoint<T>, T>  Exp (const T& x) __NE___
	{
		return std::exp( x );
	}

	template <typename T>
	ND_ EnableIf<IsFloatPoint<T>, T>  Exp2 (const T& x) __NE___
	{
		return std::exp2( x );
	}

	template <typename T>
	ND_ EnableIf<IsFloatPoint<T>, T>  Exp10 (const T& x) __NE___
	{
		return Pow( T(10), x );
	}

	template <typename T>
	ND_ EnableIf<IsFloatPoint<T>, T>  ExpMinus1 (const T& x) __NE___
	{
		return std::expm1( x );
	}

/*
=================================================
	Sign / SignOrZero
=================================================
*/
	template <typename T>
	ND_ constexpr EnableIf<IsScalar<T>, T>  Sign (const T &value) __NE___
	{
		if constexpr( IsSigned<T> )
			return value < T{0} ? T{-1} : T{1};
		else
			return T{1};
	}

	template <typename T>
	ND_ constexpr EnableIf<IsScalar<T>, T>  SignOrZero (const T &value) __NE___
	{
		if constexpr( IsSigned<T> )
			return value < T{0} ? T{-1} : value > T{0} ? T{1} : T{0};
		else
			return value > T{0} ? T{1} : T{0};
	}
	
/*
=================================================
	AnyEqual
=================================================
*/
	template <typename Lhs, typename Rhs0, typename ...Rhs>
	ND_ constexpr bool  AnyEqual (const Lhs &lhs, const Rhs0 &rhs0, const Rhs& ...rhs) __NE___
	{
		if constexpr( sizeof... (Rhs) == 0 )
			return All( lhs == rhs0 );
		else
			return All( lhs == rhs0 ) | AnyEqual( lhs, rhs... );
	}

/*
=================================================
	DivCeil
=================================================
*/
	template <typename T1, typename T2>
	ND_ constexpr T1  DivCeil (const T1 &x, const T2 &divider) __NE___
	{
		return (x + divider - T1{1}) / divider;
	}
	
/*
=================================================
	IsInfinity / IsNaN / IsFinite
=================================================
*/
	template <typename T>
	ND_ bool  IsInfinity (const T &x) __NE___
	{
		return std::isinf( x );
	}

	template <typename T>
	ND_ bool  IsNaN (const T &x) __NE___
	{
		return std::isnan( x );
	}

	template <typename T>
	ND_ bool  IsFinite (const T &x) __NE___
	{
		return std::isfinite( x );
	}
	
/*
=================================================
	Exchange
=================================================
*/
	template <typename T>
	ND_ constexpr T  Exchange (INOUT T &lhs, const T &rhs)
	{
		T	tmp = lhs;
		lhs = rhs;
		return tmp;
	}


} // AE::Math
