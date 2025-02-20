// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	SIMD version of 'base/Math/FloatConversion.h'
*/

#pragma once

namespace AE::Base
{
struct SimdFloatConversion
{
	using EMode = FloatConversion::EMode;

/*
=================================================
	FloatToHalf
=================================================
*/
public:
# if defined(AE_SIMD_SimdFloat4) and defined(AE_SIMD_SimdTInt128)
	Nd__IF static SimdUShort8  FloatToHalf (SimdFloat4 srcF) __NE___
	{
		return _FloatToHalf< SimdUShort8, SimdUInt4, SimdInt4 >( srcF );
	}
# endif
# if 0 //defined(AE_SIMD_SimdFloat8) and defined(AE_SIMD_SimdTInt256)
	Nd__IF static SimdUShort8  FloatToHalf (SimdFloat8 srcF) __NE___
	{
		return _FloatToHalf< SimdUShort8, SimdUInt8, SimdInt8 >( srcF );
	}
# elif defined(AE_SIMD_SimdFloat8)
	Nd__IF static SimdUShort8  FloatToHalf (SimdFloat8 srcF) __NE___
	{
		return	_FloatToHalf< SimdUShort8, SimdUInt4, SimdInt4 >( srcF.Lane<0>() )
					.Shuffle< 0,1,2,3, 8,9,10,11 >( _FloatToHalf< SimdUShort8, SimdUInt4, SimdInt4 >( srcF.Lane<1>() ));
	}
# endif

private:
	template <typename UShortV, typename UIntV, typename IntV, typename FloatV>
	Nd__IF static UShortV  _FloatToHalf (const FloatV srcF) __NE___
	{
		constexpr uint	FMan	= 23;
		constexpr uint	HMan	= 10;

		constexpr uint	FSign	= 23 + 8;
		constexpr uint	HSign	= 10 + 5;

		constexpr uint	FNanE	= (1u << 8) - 1;
		constexpr uint	HNanE	= (1u << 5) - 1;

		constexpr uint	ManBitsDelta = FMan - HMan;

		UIntV	src		= srcF.template BitCast<UIntV>();
		UIntV	src_e	= src.template RShift_Logic< FMan >().And( FNanE );
		UIntV	m		= src.And( (1u<<FMan)-1 );
		IntV	e		= src_e.template BitCast<IntV>() - (127 - 15);

		// round
		{
			m = m + m.And( 1u<<(ManBitsDelta-1) ).template LShift_Logic< 1 >();

			UIntV	non_ovf = m.And( 1u<<FMan ).IsZero();	// mantissa overflow

			m = Select( non_ovf, m, UIntV{} );
			e = Select( non_ovf, e, e+1 );
		}

		m = m.template RShift_Logic< ManBitsDelta >().And( (1u<<HMan)-1 );

		UIntV	v0		= m | e.Max( 0 ).template BitCast<UIntV>().template LShift_Logic< HMan >();	// default
		UIntV	v1		= m.Or( HNanE << HMan );													// nan/inf

		UIntV	c0		= src_e == UIntV{FNanE};
		UIntV	c1		= src.And( (1u<<FSign)-1 ).IsZero();

		UIntV	s0		= Select( c0, v1, v0 );
		UIntV	s1		= Select( c1, UIntV{}, s0 );

		UIntV	sign	= src.And( 1u<<FSign ).template RShift_Logic< FSign - HSign >();

		if constexpr( UShortV::count == FloatV::count )
			return s1.Or( sign ).ToShort();
		else
			return UShortV{ s1.Or( sign ).ToShort() };
	}

/*
=================================================
	HalfToFloat
=================================================
*/
public:
# if defined(AE_SIMD_SimdFloat4) and defined(AE_SIMD_SimdTInt128)
	Nd__IF static SimdFloat4  HalfToFloat (SimdUShort8 srcH) __NE___
	{
		return _HalfToFloat< SimdFloat4, SimdUInt4 >( srcH.ToInt() );
	}
# endif
# if defined(AE_SIMD_SimdFloat8) and defined(AE_SIMD_SimdTInt256) and (AE_SIMD_AVX >= 2)
	Nd__IF static SimdFloat8  HalfToFloat8 (SimdUShort8 srcH) __NE___
	{
		return _HalfToFloat< SimdFloat8, SimdUInt8 >( SimdUInt8{ srcH.ToInt<0>(), srcH.ToInt<1>() });
	}
# elif defined(AE_SIMD_SimdFloat8)
	Nd__IF static SimdFloat8  HalfToFloat8 (SimdUShort8 srcH) __NE___
	{
		return SimdFloat8{	_HalfToFloat< SimdFloat4, SimdUInt4 >( srcH.ToInt<0>() ),
							_HalfToFloat< SimdFloat4, SimdUInt4 >( srcH.ToInt<1>() )};
	}
# endif

private:
	template <typename FloatV, typename UIntV>
	Nd__IF static FloatV  _HalfToFloat (const UIntV src) __NE___
	{
		constexpr uint	FMan	= 23;
		constexpr uint	HMan	= 10;

		constexpr uint	FSign	= 23 + 8;
		constexpr uint	HSign	= 10 + 5;

		constexpr uint	FNanE	= (1u << 8) - 1;
		constexpr uint	HNanE	= (1u << 5) - 1;

		UIntV	src_e	= src.template RShift_Logic< HMan >().And( HNanE );
		UIntV	dst_e	= src_e.Add( 127 - 15 ).And( FNanE ).template LShift_Logic< FMan >();
		UIntV	dst_m	= src.And( (1u<<HMan)-1 ).template LShift_Logic< FMan - HMan >();

		UIntV	v0		= dst_e | dst_m;				// default
		UIntV	v1		= dst_m.Or( FNanE << FMan );	// nan/inf

		UIntV	c0		= src_e == UIntV{HNanE};
		UIntV	c1		= src.And( (1u<<HSign)-1 ).IsZero();

		UIntV	s0		= Select( c0, v1, v0 );
		UIntV	s1		= Select( c1, UIntV{}, s0 );

		UIntV	sign	= src.And( 1u<<HSign ).template LShift_Logic< FSign - HSign >();

		return s1.Or( sign ).template BitCast< FloatV >();
	}

}; // SimdFloatConversion
} // AE::Base
