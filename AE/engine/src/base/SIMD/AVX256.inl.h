// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/SIMD/AVX256.h"

namespace AE::Base
{

#ifdef AE_SIMD_SimdFloat8
/*
=================================================
	Bool8 ctor
=================================================
*/
# ifdef AE_SIMD_SimdTInt256
	inline SimdFloat8::Bool8::Bool8 (const SimdInt_t &v)  __NE___ : Bool8{v.Ref()} {}
	inline SimdFloat8::Bool8::Bool8 (const SimdUInt_t &v) __NE___ : Bool8{v.Ref()} {}

	inline SimdFloat8::Bool8::Bool8 (bool v0, bool v1, bool v2, bool v3, bool v4, bool v5, bool v6, bool v7) __NE___ :
		Bool8{ SimdUInt_t{ v0, v1, v2, v3, v4, v5, v6, v7 }} {}

# else
	inline SimdFloat8::Bool8::Bool8 (bool v0, bool v1, bool v2, bool v3, bool v4, bool v5, bool v6, bool v7) __NE___
	{
		const float	t	= Base::BitCast<float>( ~0u );
		const float	f	= 0.f;
		_value = _mm256_set_ps( (v7?t:f), (v6?t:f), (v5?t:f), (v4?t:f), (v3?t:f), (v2?t:f), (v1?t:f), (v0?t:f) );
	}
# endif
/*
=================================================
	constructor
=================================================
*/
	inline SimdFloat8::SimdFloat8 (const SimdFloat4 &low) __NE___ :
		_value{ _mm256_castps128_ps256( low.Ref() )}
	{}

	inline SimdFloat8::SimdFloat8 (const SimdFloat4 &low, const SimdFloat4 &high) __NE___ :
		_value{ _mm256_set_m128( high.Ref(), low.Ref() )}
	{}

/*
=================================================
	get
=================================================
*/
	template <uint I>
	float  SimdFloat8::get ()  C_NE___
	{
		StaticAssert( I < count );
		if constexpr( I < 4 )
			return SimdFloat4{ _mm256_castps256_ps128( _value )}.get<I&3>();
		else
			return SimdFloat4{ _mm256_extractf128_ps( _value, 1 )}.get<I&3>();
	}

/*
=================================================
	set
=================================================
*/
	template <uint I>
	SimdFloat8  SimdFloat8::set (Scalar_t newValue)  C_NE___
	{
		StaticAssert( I < count );
		return Select( Bool8{ I==0, I==1, I==2, I==3, I==4, I==5, I==6, I==7 }, Self{newValue}, *this );
	}

/*
=================================================
	BitCast
=================================================
*/
	template <typename DstType>
	DstType  SimdFloat8::BitCast ()  C_NE___
	{
		StaticAssert( sizeof(Self) == sizeof(DstType) );
		StaticAssert( Has_BitCast<DstType>() );

	  #ifdef AE_SIMD_SimdTInt256
		if constexpr( IsSpecializationOf< DstType, SimdTInt256 >)
			return DstType{ _mm256_castps_si256( _value )};
	  #endif
	  #ifdef AE_SIMD_Int256b
		if constexpr( IsSame< DstType, Int256b >)
			return DstType{ _mm256_castps_si256( _value )};
	  #endif
	}

/*
=================================================
	Has_BitCast
=================================================
*/
	template <typename DstType>
	__Ce__ bool  SimdFloat8::Has_BitCast ()
	{
	  #ifdef AE_SIMD_SimdTInt256
		if constexpr( IsSpecializationOf< DstType, SimdTInt256 >)
			return true;
		else
	  #endif
	  #ifdef AE_SIMD_Int256b
		if constexpr( IsSame< DstType, Int256b >)
			return true;
		else
	  #endif
			return false;
	}

/*
=================================================
	ToInt
=================================================
*/
# ifdef AE_SIMD_SimdTInt256
	inline SimdInt8  SimdFloat8::ToInt () C_NE___
	{
		return SimdInt8{_mm256_cvtps_epi32( _value )};
	}
# endif
/*
=================================================
	ToHalf
=================================================
*/
# ifdef AE_SIMD_SimdHalf8
	inline SimdHalf8  SimdFloat8::ToHalf () C_NE___
	{
		return SimdHalf8{cvtps256_ph( _value )};
	}
# endif
/*
=================================================
	Convert
----
	same as C++ style conversion 'Dst(src)'
=================================================
*/
	template <typename DstScalar>
	auto  SimdFloat8::Convert ()  C_NE___
	{
		StaticAssert( IsAnyScalar< DstScalar >);
		StaticAssert( not IsSame< DstScalar, Scalar_t >);
		StaticAssert( Has_Convert< DstScalar >() );

	  #ifdef AE_SIMD_SimdHalf8
		if constexpr( IsSame< DstScalar, half >)
			return ToHalf();
	  #endif

		if constexpr( IsSame< DstScalar, double >)
		{
			return std::array{ ToDouble<0>(), ToDouble<1>() };
		}

	  #ifdef AE_SIMD_SimdTInt256
		if constexpr( IsSame< DstScalar, int >)
			return ToInt();
		else
		if constexpr( IsInteger< DstScalar >)
			return ToInt().Convert< DstScalar >();
	  #endif
	}

/*
=================================================
	Has_Convert
=================================================
*/
	template <typename DstScalar>
	__Ce__ bool  SimdFloat8::Has_Convert ()
	{
		if constexpr( IsSame< DstScalar, Scalar_t >)
			return false;
		else
	  #ifdef AE_SIMD_SimdHalf8
		if constexpr( IsSame< DstScalar, half >)
			return true;
		else
	  #endif
		if constexpr( IsSame< DstScalar, double >)
			return true;
		else
	  #ifdef AE_SIMD_SimdTInt256
		if constexpr( IsSame< DstScalar, int >)
			return true;
		else
		if constexpr( IsInteger< DstScalar >)
			return SimdInt8::Has_Convert<DstScalar>();
		else
	  #endif
			return false;
	}

/*
=================================================
	Swizzle
=================================================
*/
#if AE_SIMD_AVX >= 2
	template <uint V0, uint V1, uint V2, uint V3, uint V4, uint V5, uint V6, uint V7>
	SimdFloat8  SimdFloat8::Swizzle () C_NE___
	{
		StaticAssert( Has_Swizzle() );
		StaticAssert( V0 < count );
		StaticAssert( V1 < count );
		StaticAssert( V2 < count );
		StaticAssert( V3 < count );
		StaticAssert( V4 < count );
		StaticAssert( V5 < count );
		StaticAssert( V6 < count );
		StaticAssert( V7 < count );
		
		using Req	= UIntSequence< V0, V1, V2, V3, V4, V5, V6, V7 >;
		using ReqHi	= UIntSequence< V4, V5, V6, V7 >;
		using ReqLo	= UIntSequence< V0, V1, V2, V3 >;
		using DefHi	= UIntSequence< 4, 5, 6, 7 >;
		using DefLo	= UIntSequence< 0, 1, 2, 3 >;

		constexpr bool	is_def_hi = IsSame< ReqHi, DefHi >;
		constexpr bool	is_def_lo = IsSame< ReqLo, DefLo >;

		if constexpr( is_def_lo and is_def_hi )
		{
			return *this;
		}else
		if constexpr( IsSame< ReqLo, DefHi > and IsSame< ReqHi, DefLo >)
		{
			return Self{_mm256_permute2f128_ps( _value, _value, 1 )};
		}else
		if constexpr( IsSame< Req, UIntSequence< V0,V0,V0,V0, V0,V0,V0,V0 >>)
		{
		  #if AE_SIMD_AVX >= 2
			constexpr uint i = V0&3;
			auto	a = _mm256_extractf128_ps( _value, V0/4 );
			auto	b = _mm_permute_ps( a, _MM_SHUFFLE(i,i,i,i) );
			return Self{_mm256_broadcastss_ps( b )};
		  #else
			return Self{ this->get<V0>() };
		  #endif
		}else
		if constexpr( V0 <= 3 and V1 <= 3 and V2 <= 3 and V3 <= 3 and
					  V4 >= 4 and V5 >= 4 and V6 >= 4 and V7 >= 4 )
		{
			if constexpr( V0+4 == V4 and V1+4 == V5 and V2+4 == V6 and V3+4 == V7 )
				return Self{_mm256_permute_ps( _value, _MM_SHUFFLE( V3, V2, V1, V0 ))};
		}else
		if constexpr( V4 <= 3 and V5 <= 3 and V6 <= 3 and V7 <= 3 and
					  V0 >= 4 and V1 >= 4 and V2 >= 4 and V3 >= 4 )
		{
			auto	lohi = _mm256_permute2f128_ps( _value, _value, 1 );
			if constexpr( V4+4 == V0 and V5+4 == V1 and V6+4 == V2 and V7+4 == V3 )
				return Self{_mm256_permute_ps( lohi, _MM_SHUFFLE( V7, V6, V5, V4 ))};
		}else
		if constexpr( IsSame< Req, UIntSequence< 1,1, 3,3, 5,5, 7,7 >>)
		{
			return Self{ _mm256_movehdup_ps( _value )};
		}else
		if constexpr( IsSame< Req, UIntSequence< 0,0, 2,2, 4,4, 6,6 >>)
		{
			return Self{ _mm256_moveldup_ps( _value )};
		}
		
		// AVX2
		__m256i indices = _mm256_set_epi32( V7, V6, V5, V4, V3, V2, V1, V0 );
		return Self{ _mm256_permutevar8x32_ps( _value, indices )};
	}
#endif

/*
=================================================
	Shuffle
=================================================
*/
	template <uint V0, uint V1, uint V2, uint V3, uint V4, uint V5, uint V6, uint V7>
	SimdFloat8  SimdFloat8::Shuffle (const Self &) C_NE___
	{
		StaticAssert( Has_Shuffle() );
		StaticAssert( V0 < count*2 );
		StaticAssert( V1 < count*2 );
		StaticAssert( V2 < count*2 );
		StaticAssert( V3 < count*2 );
		StaticAssert( V4 < count*2 );
		StaticAssert( V5 < count*2 );
		StaticAssert( V6 < count*2 );
		StaticAssert( V7 < count*2 );

		// TODO
		return {};
	}

/*
=================================================
	SwizzleLanes
=================================================
*/
	template <uint Low, uint High>
	SimdFloat8  SimdFloat8::SwizzleLanes () C_NE___
	{
		StaticAssert( Low  < lanes );
		StaticAssert( High < lanes );
		return Self{_mm256_permute2f128_ps( _value, _value, Low | (High<<4) )};
	}

/*
=================================================
	PrefixSum
=================================================
*/
	inline SimdFloat8  SimdFloat8::Sum ()  C_NE___
	{
		StaticAssert( Has_PrefixSum() );
		auto	lohi = _mm256_permute2f128_ps( _value, _value, 1 );
		auto	a	 = _mm256_hadd_ps( _value, lohi );						// 0+1, 2+3, 4+5, 6+7, ...
		auto	a1	 = _mm256_permute_ps( a, _MM_SHUFFLE( 2, 3, 0, 1 ));
		auto	b	 = _mm256_add_ps( a, a1 );								// 0+1+2+3, ..., 4+5+6+7, ...
		auto	b1	 = _mm256_permute_ps( b, _MM_SHUFFLE( 1, 0, 3, 2 ));
		auto	c	 = _mm256_add_ps( b, b1 );
		return Self{c};
	}

	inline float  SimdFloat8::PrefixSum ()  C_NE___
	{
		return Sum().get<0>();
	}

/*
=================================================
	PrefixMax
=================================================
*/
	inline SimdFloat8  SimdFloat8::Max ()  C_NE___
	{
		StaticAssert( Has_PrefixMinMax() );
		auto	lohi = _mm256_permute2f128_ps( _value, _value, 1 );
		auto	a	 = _mm256_max_ps( _value, lohi );						// max(0,4), max(1,5), max(2,6), max(3,7), ...
		auto	a1	 = _mm256_permute_ps( a, _MM_SHUFFLE( 2, 3, 0, 1 ));
		auto	b	 = _mm256_max_ps( a, a1 );								// max(0,4,1,5), ..., max(2,6,3,7), ...
		auto	b1	 = _mm256_permute_ps( b, _MM_SHUFFLE( 1, 0, 3, 2 ));
		auto	c	 = _mm256_max_ps( b, b1 );
		return Self{c};
	}

	inline float  SimdFloat8::PrefixMax ()  C_NE___
	{
		return Max().get<0>();
	}

/*
=================================================
	PrefixMin
=================================================
*/
	inline SimdFloat8  SimdFloat8::Min ()  C_NE___
	{
		StaticAssert( Has_PrefixMinMax() );
		auto	lohi = _mm256_permute2f128_ps( _value, _value, 1 );
		auto	a	 = _mm256_min_ps( _value, lohi );						// min(0,4), min(1,5), min(2,6), min(3,7), ...
		auto	a1	 = _mm256_permute_ps( a, _MM_SHUFFLE( 2, 3, 0, 1 ));
		auto	b	 = _mm256_min_ps( a, a1 );								// min(0,4,1,5), ..., min(2,6,3,7), ...
		auto	b1	 = _mm256_permute_ps( b, _MM_SHUFFLE( 1, 0, 3, 2 ));
		auto	c	 = _mm256_min_ps( b, b1 );
		return Self{c};
	}

	inline float  SimdFloat8::PrefixMin ()  C_NE___
	{
		return Min().get<0>();
	}

#endif // AE_SIMD_SimdFloat8
//-----------------------------------------------------------------------------



#ifdef AE_SIMD_SimdDouble4
/*
=================================================
	Bool4 ctor
=================================================
*/
# ifdef AE_SIMD_SimdTInt256
	inline SimdDouble4::Bool4::Bool4 (const SimdInt_t &v)  __NE___ : Bool4{v.Ref()} {}
	inline SimdDouble4::Bool4::Bool4 (const SimdUInt_t &v) __NE___ : Bool4{v.Ref()} {}

	inline SimdDouble4::Bool4::Bool4 (bool v0, bool v1, bool v2, bool v3) __NE___ :
		Bool4{ SimdUInt_t{ v0, v1, v2, v3 }} {}
# else
	inline SimdDouble4::Bool4::Bool4 (bool v0, bool v1, bool v2, bool v3) __NE___
	{
		double	t	= Base::BitCast<double>( ~0ull );
		double	f	= 0.0;
		_value = _mm256_set_pd( (v3?t:f), (v2?t:f), (v1?t:f), (v0?t:f) );
	}
# endif
/*
=================================================
	constructor
=================================================
*/
	inline SimdDouble4::SimdDouble4 (const SimdDouble2 &low) __NE___ :
		_value{ _mm256_castpd128_pd256( low.Ref() )}
	{}

	inline SimdDouble4::SimdDouble4 (const SimdDouble2 &low, const SimdDouble2 &high) __NE___ :
		_value{ _mm256_set_m128d( high.Ref(), low.Ref() )}
	{}

/*
=================================================
	get
=================================================
*/
	template <uint I>
	double  SimdDouble4::get ()  C_NE___
	{
		StaticAssert( I < count );
		if constexpr( I < 2 )
			return SimdDouble2{ _mm256_castpd256_pd128( _value )}.get<I&1>();
		else
			return SimdDouble2{ _mm256_extractf128_pd( _value, 1 )}.get<I&1>();
	}

/*
=================================================
	set
=================================================
*/
	template <uint I>
	SimdDouble4  SimdDouble4::set (Scalar_t newValue)  C_NE___
	{
		StaticAssert( I < count );
		return Select( Bool4{ I==0, I==1, I==2, I==3 }, Self{newValue}, *this );
	}

/*
=================================================
	Swizzle
=================================================
*/
#if AE_SIMD_AVX >= 2
	template <uint X, uint Y, uint Z, uint W>
	SimdDouble4  SimdDouble4::Swizzle ()  C_NE___
	{
		StaticAssert( Has_Swizzle() );
		StaticAssert( X < count );
		StaticAssert( Y < count );
		StaticAssert( Z < count );
		StaticAssert( W < count );

		constexpr int mask = (W << 6) | (Z << 4) | (Y << 2) | X;
		return Self{ _mm256_permute4x64_pd( _value, mask )};
	}
#endif

/*
=================================================
	Shuffle
=================================================
*/
	template <uint X, uint Y, uint Z, uint W>
	SimdDouble4  SimdDouble4::Shuffle (const SimdDouble4 &)  C_NE___
	{
		StaticAssert( Has_Shuffle() );
		StaticAssert( X < count*2 );
		StaticAssert( Y < count*2 );
		StaticAssert( Z < count*2 );
		StaticAssert( W < count*2 );

		// TODO
		return {};
	}

/*
=================================================
	SwizzleLanes
=================================================
*/
	template <uint Low, uint High>
	SimdDouble4  SimdDouble4::SwizzleLanes () C_NE___
	{
		StaticAssert( Low  < lanes );
		StaticAssert( High < lanes );
		return Self{_mm256_permute2f128_pd( _value, _value, Low | (High<<4) )};
	}

/*
=================================================
	BitCast
=================================================
*/
	template <typename DstType>
	DstType  SimdDouble4::BitCast ()  C_NE___
	{
		StaticAssert( sizeof(Self) == sizeof(DstType) );
		StaticAssert( Has_BitCast<DstType>() );

	  #ifdef AE_SIMD_SimdTInt256
		if constexpr( IsSpecializationOf< DstType, SimdTInt256 >)
			return DstType{ _mm256_castpd_si256( _value )};
	  #endif
	  #ifdef AE_SIMD_Int256b
		if constexpr( IsSame< DstType, Int256b >)
			return DstType{ _mm256_castpd_si256( _value )};
	  #endif
	}

/*
=================================================
	Has_BitCast
=================================================
*/
	template <typename DstType>
	__Ce__ bool  SimdDouble4::Has_BitCast ()
	{
	  #ifdef AE_SIMD_SimdTInt256
		if constexpr( IsSpecializationOf< DstType, SimdTInt256 >)
			return true;
		else
	  #endif
	  #ifdef AE_SIMD_Int256b
		if constexpr( IsSame< DstType, Int256b >)
			return true;
		else
	  #endif
			return false;
	}

/*
=================================================
	Reciprocal_fp32
=================================================
*/
	inline SimdDouble4  SimdDouble4::Reciprocal_fp32 () C_NE___
	{
		__m128	f01 = _mm256_cvtpd_ps( _value );
		__m128	rcp = _mm_rcp_ps( f01 );
		return Self{_mm256_cvtps_pd( rcp )};
	}

/*
=================================================
	ToFloat / ToLong / ToInt
=================================================
*/
	inline SimdFloat4  SimdDouble4::ToFloat () C_NE___
	{
		return SimdFloat4{_mm256_cvtpd_ps( _value )};
	}

	inline SimdInt4  SimdDouble4::ToInt () C_NE___
	{
		return SimdInt4{_mm256_cvtpd_epi32( _value )};
	}

# if AE_SIMD_AVX >= 31 // AVX512DQ, AVX512VL
	inline SimdLong4  SimdDouble4::ToLong () C_NE___
	{
		return SimdLong4{_mm256_cvtpd_epi64( _value )};
	}
# endif
/*
=================================================
	Convert
----
	same as C++ style conversion 'Dst(src)'
=================================================
*/
	template <typename DstScalar>
	auto  SimdDouble4::Convert ()  C_NE___
	{
		StaticAssert( IsAnyScalar< DstScalar >);
		StaticAssert( not IsSame< DstScalar, Scalar_t >);
		StaticAssert( Has_Convert< DstScalar >() );

		if constexpr( IsSame< DstScalar, float >)
			return ToFloat();
		else
		if constexpr( IsSame< DstScalar, int >)
			return ToInt();
		else
	  #if AE_SIMD_AVX >= 31 // AVX512DQ, AVX512VL
		if constexpr( IsSame< DstScalar, slong >)
			return ToLong();
		else
		if constexpr( IsInteger< DstScalar >)
			return ToLong().Convert< DstScalar >();
		else
	  #endif
		if constexpr( IsInteger< DstScalar >)
			return ToInt().Convert< DstScalar >();
	}

/*
=================================================
	Has_Convert
=================================================
*/
	template <typename DstScalar>
	__Ce__ bool  SimdDouble4::Has_Convert ()
	{
		if constexpr( IsSame< DstScalar, Scalar_t >)
			return false;
		else
		if constexpr( IsSame< DstScalar, float >)
			return true;
		else
		if constexpr( IsSame< DstScalar, int >)
			return true;
		else
	  #if AE_SIMD_AVX >= 31 // AVX512DQ, AVX512VL
		if constexpr( IsSame< DstScalar, slong >)
			return true;
		else
	  #endif
		if constexpr( IsInteger< DstScalar >)
			return SimdInt4::Has_Convert<DstScalar>();
		else
			return false;
	}

/*
=================================================
	PrefixSum
=================================================
*/
	inline SimdDouble4  SimdDouble4::Sum ()  C_NE___
	{
		StaticAssert( Has_PrefixSum() );
		auto	a = this->Add( this->Swizzle<1,0,3,2>() );
		return a.Add( a.Swizzle<2,3,0,1>() );
	}

	inline double  SimdDouble4::PrefixSum ()  C_NE___
	{
		return Sum().get<0>();
	}

/*
=================================================
	PrefixMax
=================================================
*/
	inline SimdDouble4  SimdDouble4::Max ()  C_NE___
	{
		StaticAssert( Has_PrefixMinMax() );
		auto	a = this->Max( this->Swizzle<1,0,3,2>() );
		return a.Max( a.Swizzle<2,3,0,1>() );
	}

	inline double  SimdDouble4::PrefixMax ()  C_NE___
	{
		return Max().get<0>();
	}

/*
=================================================
	PrefixMin
=================================================
*/
	inline SimdDouble4  SimdDouble4::Min ()  C_NE___
	{
		StaticAssert( Has_PrefixMinMax() );
		auto	a = this->Min( this->Swizzle<1,0,3,2>() );	// min(1,0), min(0,1), min(3,2), min(2,3)
		return a.Min( a.Swizzle<2,3,0,1>() );				// min(1,0,3,2)
	}

	inline double  SimdDouble4::PrefixMin ()  C_NE___
	{
		return Min().get<0>();
	}

#endif // AE_SIMD_SimdDouble4
//-----------------------------------------------------------------------------



#ifdef AE_SIMD_SimdTInt256
/*
=================================================
	constructor
=================================================
*/
	template <typename IT>
	SimdTInt256<IT>::SimdTInt256 (Scalar_t v) __NE___
	{
		if constexpr( is8 )		_value = _mm256_set1_epi8(   v );	else
		if constexpr( is16 )	_value = _mm256_set1_epi16(  v );	else
		if constexpr( is32 )	_value = _mm256_set1_epi32(  v );	else
		if constexpr( is64 )	_value = _mm256_set1_epi64x( v );	else
		if constexpr( is128 )	_value = _mm256_set_m128i( v, v );	else
								_value = v; // compilation error
	}

	template <typename IT>
	template <typename B, typename T> requires( IsSame< T, ubyte > and IsSame< B, bool >)
	SimdTInt256<IT>::SimdTInt256 (B v00, B v01, B v02, B v03, B v04, B v05, B v06, B v07,
								  B v08, B v09, B v10, B v11, B v12, B v13, B v14, B v15,
								  B v16, B v17, B v18, B v19, B v20, B v21, B v22, B v23,
								  B v24, B v25, B v26, B v27, B v28, B v29, B v30, B v31) __NE___ :
		SimdTInt256{ T(v00 ? 0xFF : 0), T(v01 ? 0xFF : 0), T(v02 ? 0xFF : 0), T(v03 ? 0xFF : 0),
					 T(v04 ? 0xFF : 0), T(v05 ? 0xFF : 0), T(v06 ? 0xFF : 0), T(v07 ? 0xFF : 0),
					 T(v08 ? 0xFF : 0), T(v09 ? 0xFF : 0), T(v10 ? 0xFF : 0), T(v11 ? 0xFF : 0),
					 T(v12 ? 0xFF : 0), T(v13 ? 0xFF : 0), T(v14 ? 0xFF : 0), T(v15 ? 0xFF : 0),
					 T(v16 ? 0xFF : 0), T(v17 ? 0xFF : 0), T(v18 ? 0xFF : 0), T(v19 ? 0xFF : 0),
					 T(v20 ? 0xFF : 0), T(v21 ? 0xFF : 0), T(v22 ? 0xFF : 0), T(v23 ? 0xFF : 0),
					 T(v24 ? 0xFF : 0), T(v25 ? 0xFF : 0), T(v26 ? 0xFF : 0), T(v27 ? 0xFF : 0),
					 T(v28 ? 0xFF : 0), T(v29 ? 0xFF : 0), T(v30 ? 0xFF : 0), T(v31 ? 0xFF : 0) }
	{}

	template <typename IT>
	template <typename B, typename T> requires( IsSame< T, ushort > and IsSame< B, bool >)
	SimdTInt256<IT>::SimdTInt256 (B v00, B v01, B v02, B v03, B v04, B v05, B v06, B v07,
								  B v08, B v09, B v10, B v11, B v12, B v13, B v14, B v15) __NE___ :
		SimdTInt256{ T(v00 ? 0xFFFF : 0), T(v01 ? 0xFFFF : 0), T(v02 ? 0xFFFF : 0), T(v03 ? 0xFFFF : 0),
					 T(v04 ? 0xFFFF : 0), T(v05 ? 0xFFFF : 0), T(v06 ? 0xFFFF : 0), T(v07 ? 0xFFFF : 0),
					 T(v08 ? 0xFFFF : 0), T(v09 ? 0xFFFF : 0), T(v10 ? 0xFFFF : 0), T(v11 ? 0xFFFF : 0),
					 T(v12 ? 0xFFFF : 0), T(v13 ? 0xFFFF : 0), T(v14 ? 0xFFFF : 0), T(v15 ? 0xFFFF : 0) }
	{}

	template <typename IT>
	template <typename B, typename T> requires( IsSame< T, uint > and IsSame< B, bool >)
	SimdTInt256<IT>::SimdTInt256 (B v0, B v1, B v2, B v3, B v4, B v5, B v6, B v7) __NE___ :
		SimdTInt256{ (v0 ? ~0u : 0), (v1 ? ~0u : 0), (v2 ? ~0u : 0), (v3 ? ~0u : 0),
					 (v4 ? ~0u : 0), (v5 ? ~0u : 0), (v6 ? ~0u : 0), (v7 ? ~0u : 0) }
	{}

	template <typename IT>
	template <typename B, typename T> requires( IsSame< T, ulong > and IsSame< B, bool >)
	SimdTInt256<IT>::SimdTInt256 (B v0, B v1, B v2, B v3) __NE___ :
		SimdTInt256{ (v0 ? ~0ull : 0), (v1 ? ~0ull : 0), (v2 ? ~0ull : 0), (v3 ? ~0ull : 0) }
	{}

/*
=================================================
	constructor
=================================================
*/
	template <typename IT>
	SimdTInt256<IT>::SimdTInt256 (const SimdTInt128<Scalar_t> &low) __NE___ :
		_value{ _mm256_castsi128_si256( low.Ref() )}
	{}

	template <typename IT>
	SimdTInt256<IT>::SimdTInt256 (const SimdTInt128<Scalar_t> &low, const SimdTInt128<Scalar_t> &high) __NE___ :
		_value{ _mm256_set_m128i( high.Ref(), low.Ref() )}
	{}

/*
=================================================
	get
=================================================
*/
	template <typename IT>
	template <uint I>
	IT  SimdTInt256<IT>::get ()  C_NE___
	{
		StaticAssert( I < count );

		if constexpr( is128 )
		{
			if constexpr( I == 0 )
				return Int128b{ _mm256_castsi256_si128( _value )};
			else
				return Int128b{ _mm256_extractf128_si256( _value, 1 )};
		}
		else
		{
			constexpr uint	I2 = I & (count/2-1);

			if constexpr( I < count/2 )
				return SimdTInt128<IT>{ _mm256_castsi256_si128( _value )}.template get<I2>();
			else
				return SimdTInt128<IT>{ _mm256_extractf128_si256( _value, 1 )}.template get<I2>();
		}
	}

/*
=================================================
	set
=================================================
*/
	template <typename IT>
	template <uint I>
	SimdTInt256<IT>  SimdTInt256<IT>::set (Scalar_t newValue)  C_NE___
	{
		StaticAssert( I < count );

		if constexpr( is64 )
			return Select( Unsigned_t{ I==0, I==1, I==2, I==3 }, Self{newValue}, *this );
		else
		if constexpr( is32 ){
			Unsigned_t  b{ I==0, I==1, I==2, I==3, I==4, I==5, I==6, I==7 };
			return Select( b, Self{newValue}, *this );
		}else
		if constexpr( is16 ){
			Unsigned_t  b{	I==0, I==1, I==2, I==3, I==4, I==5, I==6, I==7,
							I==8, I==9, I==10, I==11, I==12, I==13, I==14, I==15 };
			return Select( b, Self{newValue}, *this );
		}else
		if constexpr( is8 ){
			Unsigned_t  b{	I==0,  I==1,  I==2,  I==3,  I==4,  I==5,  I==6,  I==7,
							I==8,  I==9,  I==10, I==11, I==12, I==13, I==14, I==15,
							I==16, I==17, I==18, I==19, I==20, I==21, I==22, I==23,
							I==24, I==25, I==26, I==27, I==28, I==29, I==30, I==31 };
			return Select( b, Self{newValue}, *this );
		}
	}

/*
=================================================
	BitCast
=================================================
*/
	template <typename IT>
	template <typename DstType>
	DstType  SimdTInt256<IT>::BitCast ()  C_NE___
	{
		StaticAssert( sizeof(Self) == sizeof(DstType) );
		StaticAssert( Has_BitCast<DstType>() );

		if constexpr( IsSame< DstType, Int256b >)
			return DstType{ _value };

		if constexpr( IsSpecializationOf< DstType, SimdTInt256 >)
			return DstType{ _value };

		if constexpr( IsSame< DstType, SimdDouble4 >)
			return SimdDouble4{ _mm256_castsi256_pd( _value )};

		if constexpr( IsSame< DstType, SimdFloat8 >)
			return SimdFloat8{ _mm256_castsi256_ps( _value )};
	}

/*
=================================================
	Has_BitCast
=================================================
*/
	template <typename IT>
	template <typename DstType>
	__Ce__ bool  SimdTInt256<IT>::Has_BitCast ()
	{
		if constexpr( IsSame< DstType, Int256b >)
			return true;
		else
		if constexpr( IsSpecializationOf< DstType, SimdTInt256 >)
			return true;
		else
		if constexpr( IsSame< DstType, SimdDouble4 >)
			return true;
		else
		if constexpr( IsSame< DstType, SimdFloat8 >)
			return true;
		else
			return false;
	}

/*
=================================================
	ToFloat
=================================================
*/
	template <typename IT>
	template <uint Idx>
	auto  SimdTInt256<IT>::ToFloat () C_NE___
	{
		if constexpr( isI32 )
		{
			StaticAssert( Idx == 0 );
			return SimdFloat8{_mm256_cvtepi32_ps( _value )};
		}

	  #if AE_SIMD_AVX >= 2
		if constexpr( isU32 )
		{
			StaticAssert( Idx == 0 );
		  #if AE_SIMD_AVX >= 31 // AVX512VL
			return SimdFloat8{_mm256_cvtepu32_ps( _value )};
		  #else
			auto	u_lo = _mm256_and_si256( _value, _mm256_set1_epi32(0xFFFF) );
			auto	u_hi = _mm256_srli_epi32( _value, 16 );
			auto	f_lo = _mm256_cvtepi32_ps( u_lo );
			auto	f_hi = _mm256_cvtepi32_ps( u_hi );
					f_hi = _mm256_mul_ps( _mm256_set1_ps(65536.0f), f_hi );
			return SimdFloat8{_mm256_add_ps( f_hi, f_lo )};
		  #endif
		}

		if constexpr( is8 or is16 )
			return ToInt< Idx >().ToFloat();
	  #endif
	}

/*
=================================================
	ToDouble
=================================================
*/
	template <typename IT>
	template <uint Idx>
	auto  SimdTInt256<IT>::ToDouble () C_NE___
	{
		if constexpr( isI32 )
			return Lane<Idx>()._IntToDouble4();

		if constexpr( isU32 )
			return Lane<Idx>()._UIntToDouble4();

	  #if AE_SIMD_AVX >= 2
		if constexpr( is8 )
		{
			StaticAssert( Idx < 8 );
			return ToInt< Idx/2 >().template ToDouble< Idx&1 >();
		}

		if constexpr( is16 )
		{
			StaticAssert( Idx < 4 );
			return ToInt< Idx/2 >().template ToDouble< Idx&1 >();
		}
	  #endif
	}

/*
=================================================
	ToSigned
=================================================
*/
	template <typename IT>
	template <typename T> requires( IsUnsignedInteger<T> )
	auto  SimdTInt256<IT>::ToSigned () C_NE___
	{
		using S = ToSignedInteger<IT>;
		return BitCast< SimdTInt256<S> >().Max( S{0} );
	}

/*
=================================================
	ToUnsigned
=================================================
*/
	template <typename IT>
	template <typename T> requires( IsSignedInteger<T> )
	auto  SimdTInt256<IT>::ToUnsigned () C_NE___
	{
		using U = ToUnsignedInteger<IT>;
		return Max( IT{0} ).template BitCast< SimdTInt256<U> >();
	}

/*
=================================================
	Convert
----
	same as C++ style conversion 'Dst(src)'
=================================================
*/
	template <typename IT>
	template <typename DstScalar>
	auto  SimdTInt256<IT>::Convert ()  C_NE___
	{
		StaticAssert( IsAnyScalar< DstScalar >);
		StaticAssert( not IsSame< DstScalar, Scalar_t >);
		StaticAssert( Has_Convert< DstScalar >() );

	  #if AE_SIMD_AVX >= 2
		if constexpr( IsInteger< DstScalar >)
		{
			using D = SimdTInt256<DstScalar>;

			if constexpr( IsSame< DstScalar, ulong >)
			{
				if constexpr( isU8 )	return std::array{	ToLong<0>(), ToLong<1>(), ToLong<2>(), ToLong<3>(),
															ToLong<4>(), ToLong<5>(), ToLong<6>(), ToLong<7>() };
				if constexpr( isI8 )	return std::array{	ToLong<0>().template BitCast<D>(), ToLong<1>().template BitCast<D>(),
															ToLong<2>().template BitCast<D>(), ToLong<3>().template BitCast<D>(),
															ToLong<4>().template BitCast<D>(), ToLong<5>().template BitCast<D>(),
															ToLong<6>().template BitCast<D>(), ToLong<7>().template BitCast<D>() };
				if constexpr( isU16 )	return std::array{	ToLong<0>(), ToLong<1>(), ToLong<2>(), ToLong<3>() };
				if constexpr( isI16 )	return std::array{	ToLong<0>().template BitCast<D>(), ToLong<1>().template BitCast<D>(),
															ToLong<2>().template BitCast<D>(), ToLong<3>().template BitCast<D>() };
				if constexpr( isU32 )	return std::array{	ToLong<0>(), ToLong<1>() };
				if constexpr( isI32 )	return std::array{	ToLong<0>().template BitCast<D>(), ToLong<1>().template BitCast<D>() };
				if constexpr( isI64 )	return BitCast<D>();
			}

			if constexpr( IsSame< DstScalar, slong >)
			{
				if constexpr( isI8 )	return std::array{	ToLong<0>(), ToLong<1>(), ToLong<2>(), ToLong<3>(),
															ToLong<4>(), ToLong<5>(), ToLong<6>(), ToLong<7>() };
				if constexpr( isU8 )	return std::array{	ToLong<0>().template BitCast<D>(), ToLong<1>().template BitCast<D>(),
															ToLong<2>().template BitCast<D>(), ToLong<3>().template BitCast<D>(),
															ToLong<4>().template BitCast<D>(), ToLong<5>().template BitCast<D>(),
															ToLong<6>().template BitCast<D>(), ToLong<7>().template BitCast<D>() };
				if constexpr( isI16 )	return std::array{	ToLong<0>(), ToLong<1>(), ToLong<2>(), ToLong<3>() };
				if constexpr( isU16 )	return std::array{	ToLong<0>().template BitCast<D>(), ToLong<1>().template BitCast<D>(),
															ToLong<2>().template BitCast<D>(), ToLong<3>().template BitCast<D>() };
				if constexpr( isI32 )	return std::array{	ToLong<0>(), ToLong<1>() };
				if constexpr( isU32 )	return std::array{	ToLong<0>().template BitCast<D>(), ToLong<1>().template BitCast<D>() };
				if constexpr( isU64 )	return BitCast<D>();
			}

			if constexpr( IsSame< DstScalar, uint >)
			{
				if constexpr( isU8 )	return std::array{	ToInt<0>(), ToInt<1>(), ToInt<2>(), ToInt<3>() };
				if constexpr( isI8 )	return std::array{	ToInt<0>().template BitCast<D>(), ToInt<1>().template BitCast<D>(),
															ToInt<2>().template BitCast<D>(), ToInt<3>().template BitCast<D>() };
				if constexpr( isU16 )	return std::array{	ToInt<0>(), ToInt<1>() };
				if constexpr( isI16 )	return std::array{	ToInt<0>().template BitCast<D>(), ToInt<1>().template BitCast<D>() };
				if constexpr( isI32 )	return BitCast<D>();
			}

			if constexpr( IsSame< DstScalar, sint >)
			{
				if constexpr( isI8 )	return std::array{	ToInt<0>(), ToInt<1>(), ToInt<2>(), ToInt<3>() };
				if constexpr( isU8 )	return std::array{	ToInt<0>().template BitCast<D>(), ToInt<1>().template BitCast<D>(),
															ToInt<2>().template BitCast<D>(), ToInt<3>().template BitCast<D>() };
				if constexpr( isI16 )	return std::array{	ToInt<0>(), ToInt<1>() };
				if constexpr( isU16 )	return std::array{	ToInt<0>().template BitCast<D>(), ToInt<1>().template BitCast<D>() };
				if constexpr( isU32 )	return BitCast<D>();
			}

			if constexpr( IsSame< DstScalar, ushort >)
			{
				if constexpr( isU8  )	return std::array{	ToShort<0>(), ToShort<1>() };
				if constexpr( isI8  )	return std::array{	ToShort<0>().template BitCast<D>(), ToShort<1>().template BitCast<D>() };
				if constexpr( isI16 )	return BitCast<D>();
				//if constexpr( isU32 or isU64 )	return ToShort();
				//if constexpr( isI32 or isI64 )	return ToShort().template BitCast<D>();
			}

			if constexpr( IsSame< DstScalar, sshort >)
			{
				if constexpr( isI8  )	return std::array{	ToShort<0>(), ToShort<1>() };
				if constexpr( isU8  )	return std::array{	ToShort<0>().template BitCast<D>(), ToShort<1>().template BitCast<D>() };
				if constexpr( isU16 )	return BitCast<D>();
				//if constexpr( isI32 or isI64 )	return ToShort();
				//if constexpr( isU32 or isU64 )	return ToShort().template BitCast<D>();
			}
		}
	  #endif // AVX2

		if constexpr( IsSame< DstScalar, float >)
		{
			if constexpr( is32 )	return ToFloat();
			if constexpr( is16 )	return std::array{ ToFloat<0>(), ToFloat<1>() };
			if constexpr( is8  )	return std::array{ ToFloat<0>(), ToFloat<1>(), ToFloat<2>(), ToFloat<3>() };
		}

		if constexpr( IsSame< DstScalar, double > and is32 )
		{
		  #if 0 //AE_SIMD_AVX >= 30 // ???
			if constexpr( isI32 )	return _IntToDouble8();
			else					return _UIntToDouble8();
		  #else
			return std::array{ ToDouble<0>(), ToDouble<1>() };
		  #endif
		}

	  #if 0 //AE_SIMD_AVX >= 30 // ???
		if constexpr( IsSame< DstScalar, double > and is64 )
		{
			if constexpr( isI64 )	return _LongToDouble4();
			else					return _ULongToDouble4();
		}
	  #endif
	}

/*
=================================================
	Has_Convert
=================================================
*/
	template <typename IT>
	template <typename DstScalar>
	__Ce__ bool  SimdTInt256<IT>::Has_Convert ()
	{
		if constexpr( IsSame< DstScalar, Scalar_t >)
			return false;
		else
	  #if AE_SIMD_AVX >= 2
		if constexpr( IsInteger< DstScalar >)
			return	(sizeof(DstScalar) == sizeof(ulong)		and sizeof(IT) <= sizeof(ulong))	or
					(sizeof(DstScalar) == sizeof(uint)		and sizeof(IT) <= sizeof(uint))		or
					(sizeof(DstScalar) == sizeof(ushort)	and sizeof(IT) <= sizeof(ushort));
		else
	  #endif
		if constexpr( IsSame< DstScalar, float >)
			return isI32 or ((isU32 or is8 or is16) and AE_SIMD_AVX >= 2);
		else
		if constexpr( IsSame< DstScalar, double >)
			return is32; //or (is64 and AE_SIMD_AVX >= 30);
		else
			return false;
	}

/*
=================================================
	Select
=================================================
*/
	template <typename IT>
	SimdTInt256<IT>  SimdTInt256<IT>::_Select (const Bool_t &condition, const Self &ifTrue, const Self &ifFalse) __NE___
	{
	  #if AE_SIMD_AVX >= 2
		return Self{ _mm256_blendv_epi8( ifFalse._value, ifTrue._value, condition.Ref() )};

	  #else
		__m256	c	= condition.template BitCast< SimdFloat8 >().Ref();
		__m256	t	= ifTrue.template BitCast< SimdFloat8 >().Ref();
		__m256	f	= ifFalse.template BitCast< SimdFloat8 >().Ref();

		auto	a	= _mm256_andnot_ps( c, f );
		auto	b	= _mm256_and_ps( c, t );
		return SimdFloat8{ _mm256_or_ps( a, b )}.template BitCast< SimdTInt256<IT> >();
	  #endif
	}


# if AE_SIMD_AVX >= 2
/*
=================================================
	Add
=================================================
*/
	template <typename IT>
	SimdTInt256<IT>  SimdTInt256<IT>::Add (const Self &rhs)  C_NE___
	{
		if constexpr( is8 )		return Self{ _mm256_add_epi8(  _value, rhs._value )};
		if constexpr( is16 )	return Self{ _mm256_add_epi16( _value, rhs._value )};
		if constexpr( is32 )	return Self{ _mm256_add_epi32( _value, rhs._value )};
		if constexpr( is64 )	return Self{ _mm256_add_epi64( _value, rhs._value )};
	}

/*
=================================================
	AddSat
=================================================
*/
	template <typename IT>
	SimdTInt256<IT>  SimdTInt256<IT>::AddSat (const Self &rhs)  C_NE___
	{
		if constexpr( isI8 )	return Self{ _mm256_adds_epi8(  _value, rhs._value )};
		if constexpr( isU8 )	return Self{ _mm256_adds_epu8(  _value, rhs._value )};
		if constexpr( isI16 )	return Self{ _mm256_adds_epi16( _value, rhs._value )};
		if constexpr( isU16 )	return Self{ _mm256_adds_epu16( _value, rhs._value )};
	}

/*
=================================================
	Sub
=================================================
*/
	template <typename IT>
	SimdTInt256<IT>  SimdTInt256<IT>::Sub (const Self &rhs)  C_NE___
	{
		if constexpr( is8 )		return Self{ _mm256_sub_epi8(  _value, rhs._value )};
		if constexpr( is16 )	return Self{ _mm256_sub_epi16( _value, rhs._value )};
		if constexpr( is32 )	return Self{ _mm256_sub_epi32( _value, rhs._value )};
		if constexpr( is64 )	return Self{ _mm256_sub_epi64( _value, rhs._value )};
	}

/*
=================================================
	SubSat
=================================================
*/
	template <typename IT>
	SimdTInt256<IT>  SimdTInt256<IT>::SubSat (const Self &rhs)  C_NE___
	{
		if constexpr( isI8 )	return Self{ _mm256_subs_epi8(  _value, rhs._value )};
		if constexpr( isU8 )	return Self{ _mm256_subs_epu8(  _value, rhs._value )};
		if constexpr( isI16 )	return Self{ _mm256_subs_epi16( _value, rhs._value )};
		if constexpr( isU16 )	return Self{ _mm256_subs_epu16( _value, rhs._value )};
	}

/*
=================================================
	Mul
=================================================
*/
	template <typename IT>
	SimdTInt256<IT>  SimdTInt256<IT>::Mul (const Self &rhs)  C_NE___
	{
		StaticAssert( Has_Mul() );
		if constexpr( isI16 )	return Self{ _mm256_mullo_epi16( _value, rhs._value )};
		if constexpr( isU16 )	return Self{ _mm256_mullo_epi16( _value, rhs._value )};

		if constexpr( isI32 )	return Self{ _mm256_mullo_epi32( _value, rhs._value )};
		if constexpr( isU32 )	return Self{ _mm256_mullo_epi32( _value, rhs._value )};
	}

	template <typename IT>
	__Ce__ bool  SimdTInt256<IT>::Has_Mul ()
	{
		return is16 or is32;
	}

/*
=================================================
	Min
=================================================
*/
	template <typename IT>
	SimdTInt256<IT>  SimdTInt256<IT>::Min (const Self &rhs)  C_NE___
	{
		StaticAssert( Has_MinMax() );
		if constexpr( isI8 )	return Self{ _mm256_min_epi8(  _value, rhs._value )};
		if constexpr( isU8 )	return Self{ _mm256_min_epu8(  _value, rhs._value )};
		if constexpr( isI16 )	return Self{ _mm256_min_epi16( _value, rhs._value )};
		if constexpr( isU16 )	return Self{ _mm256_min_epu16( _value, rhs._value )};
		if constexpr( isI32 )	return Self{ _mm256_min_epi32( _value, rhs._value )};
		if constexpr( isU32 )	return Self{ _mm256_min_epu32( _value, rhs._value )};
	  #if AE_SIMD_AVX >= 31  // AVX512VL
		if constexpr( isI64 )	return Self{ _mm256_min_epi64( _value, rhs._value )};
		if constexpr( isU64 )	return Self{ _mm256_min_epu64( _value, rhs._value )};
	  #endif
	}

	template <typename IT>
	__Ce__ bool  SimdTInt256<IT>::Has_MinMax ()
	{
		return	(AE_SIMD_AVX >= 2) and
				(sizeof(IT) <= sizeof(uint) or (AE_SIMD_AVX >= 31));
	}

/*
=================================================
	Max
=================================================
*/
	template <typename IT>
	SimdTInt256<IT>  SimdTInt256<IT>::Max (const Self &rhs)  C_NE___
	{
		StaticAssert( Has_MinMax() );
		if constexpr( isI8 )	return Self{ _mm256_max_epi8(  _value, rhs._value )};
		if constexpr( isU8 )	return Self{ _mm256_max_epu8(  _value, rhs._value )};
		if constexpr( isI16 )	return Self{ _mm256_max_epi16( _value, rhs._value )};
		if constexpr( isU16 )	return Self{ _mm256_max_epu16( _value, rhs._value )};
		if constexpr( isI32 )	return Self{ _mm256_max_epi32( _value, rhs._value )};
		if constexpr( isU32 )	return Self{ _mm256_max_epu32( _value, rhs._value )};
	  #if AE_SIMD_AVX >= 31  // AVX512VL
		if constexpr( isI64 )	return Self{ _mm256_max_epi64( _value, rhs._value )};
		if constexpr( isU64 )	return Self{ _mm256_max_epu64( _value, rhs._value )};
	  #endif
	}

/*
=================================================
	Equal
=================================================
*/
	template <typename IT>
	typename SimdTInt256<IT>::Bool_t  SimdTInt256<IT>::Equal (const Self &rhs)  C_NE___
	{
		StaticAssert( Has_Equal() );
		if constexpr( is8 )		return Bool_t{ _mm256_cmpeq_epi8(  _value, rhs._value )};
		if constexpr( is16 )	return Bool_t{ _mm256_cmpeq_epi16( _value, rhs._value )};
		if constexpr( is32 )	return Bool_t{ _mm256_cmpeq_epi32( _value, rhs._value )};
		if constexpr( is64 )	return Bool_t{ _mm256_cmpeq_epi64( _value, rhs._value )};
	}

	template <typename IT>
	__Ce__ bool  SimdTInt256<IT>::Has_Equal ()
	{
		return sizeof(IT) <= sizeof(ulong);
	}

/*
=================================================
	Greater
=================================================
*/
	template <typename IT>
	typename SimdTInt256<IT>::Bool_t  SimdTInt256<IT>::Greater (const Self &rhs)  C_NE___
	{
		StaticAssert( Has_Greater() );

		if constexpr( isI8 )			return Bool_t{ _mm256_cmpgt_epi8(  _value, rhs._value )};	else
		if constexpr( isI16 )			return Bool_t{ _mm256_cmpgt_epi16( _value, rhs._value )};	else
		if constexpr( isI32 )			return Bool_t{ _mm256_cmpgt_epi32( _value, rhs._value )};	else
		if constexpr( isI64 )			return Bool_t{ _mm256_cmpgt_epi64( _value, rhs._value )};	else
		if constexpr( Has_MinMax() )	return *this != this->Min( rhs );	// not LEqual
	}

	template <typename IT>
	__Ce__ bool  SimdTInt256<IT>::_Has_Native_Greater ()
	{
		return isI8 or isI16 or isI32 or isI64;
	}

	template <typename IT>
	__Ce__ bool  SimdTInt256<IT>::Has_Greater ()
	{
		return _Has_Native_Greater() or Has_MinMax();
	}

/*
=================================================
	LEqual
----
	a <= b
=================================================
*/
	template <typename IT>
	typename SimdTInt256<IT>::Bool_t  SimdTInt256<IT>::LEqual (const Self &rhs)  C_NE___
	{
		if constexpr( Has_MinMax() )
			return *this == this->Min( rhs );
		else
		if constexpr( _Has_Native_Greater() )
			return not Greater( rhs );
	}

/*
=================================================
	LShift_Logic
----
	Used scalar shift for whole vector.
	Returns:
	 - zero element on overflow
	 - zero for negative vector shift.
=================================================
*/
	template <typename IT>
	template <uint Shift>
	SimdTInt256<IT>  SimdTInt256<IT>::LShift_Logic ()  C_NE___
	{
		StaticAssert( Shift <= CT_SizeOfInBits<Scalar_t> );
		return LShift_Logic( Shift );
	}

	template <typename IT>
	SimdTInt256<IT>  SimdTInt256<IT>::LShift_Logic (uint shift)  C_NE___
	{
		StaticAssert( Has_ScalarShift_Logic() );
		ASSERT_MSG( shift <= CT_SizeOfInBits<Scalar_t>, "Result will be zero" );

		if constexpr( is16 )	return Self{ _mm256_slli_epi16( _value, shift )};
		if constexpr( is32 )	return Self{ _mm256_slli_epi32( _value, shift )};
		if constexpr( is64 )	return Self{ _mm256_slli_epi64( _value, shift )};
	}

	template <typename IT>
	SimdTInt256<IT>  SimdTInt256<IT>::LShift_Logic (const Shift64_t &shift)  C_NE___
	{
		StaticAssert( Has_ScalarShift_Logic() );
		ASSERT_MSG( shift.get<0>() >= 0, "Result will be zero" );
		ASSERT_MSG( shift.get<0>() <= int(CT_SizeOfInBits<Scalar_t>), "Result will be zero" );

		if constexpr( is16 )	return Self{ _mm256_sll_epi16( _value, shift.Ref() )};
		if constexpr( is32 )	return Self{ _mm256_sll_epi32( _value, shift.Ref() )};
		if constexpr( is64 )	return Self{ _mm256_sll_epi64( _value, shift.Ref() )};
	}

	template <typename IT>
	__Ce__ bool  SimdTInt256<IT>::Has_ScalarShift_Logic ()
	{
		return is16 or is32 or is64;
	}

/*
=================================================
	RShift_Logic
----
	Used scalar shift for whole vector.
	Returns:
	 - zero element on overflow
	 - zero for negative vector shift.
=================================================
*/
	template <typename IT>
	template <uint Shift>
	SimdTInt256<IT>  SimdTInt256<IT>::RShift_Logic ()  C_NE___
	{
		StaticAssert( Shift > 0 );
		StaticAssert( Shift <= CT_SizeOfInBits<Scalar_t> );
		return RShift_Logic( Shift );
	}

	template <typename IT>
	SimdTInt256<IT>  SimdTInt256<IT>::RShift_Logic (uint shift)  C_NE___
	{
		StaticAssert( Has_ScalarShift_Logic() );
		ASSERT_MSG( shift <= CT_SizeOfInBits<Scalar_t>, "Result will be zero" );

		if constexpr( is16 )	return Self{ _mm256_srli_epi16( _value, shift )};
		if constexpr( is32 )	return Self{ _mm256_srli_epi32( _value, shift )};
		if constexpr( is64 )	return Self{ _mm256_srli_epi64( _value, shift )};
	}

	template <typename IT>
	SimdTInt256<IT>  SimdTInt256<IT>::RShift_Logic (const Shift64_t &shift)  C_NE___
	{
		StaticAssert( Has_ScalarShift_Logic() );
		ASSERT_MSG( shift.get<0>() >= 0, "Result will be zero" );
		ASSERT_MSG( shift.get<0>() <= int(CT_SizeOfInBits<Scalar_t>), "Result will be zero" );

		if constexpr( is16 )	return Self{ _mm256_srl_epi16( _value, shift.Ref() )};
		if constexpr( is32 )	return Self{ _mm256_srl_epi32( _value, shift.Ref() )};
		if constexpr( is64 )	return Self{ _mm256_srl_epi64( _value, shift.Ref() )};
	}

/*
=================================================
	RShift_Arith
----
	Used scalar shift for whole vector.
	Returns:
	 - max element on overflow
	 - zero for negative shift on positive value
	 - -1 for negative shift on negative value.
	Used for negative values: '-800 >> 2 = -200'
=================================================
*/
	template <typename IT>
	template <uint Shift>
	SimdTInt256<IT>  SimdTInt256<IT>::RShift_Arith ()  C_NE___
	{
		if constexpr( IsSigned<IT> )
		{
			StaticAssert( Shift <= CT_SizeOfInBits<Scalar_t> );
			return RShift_Arith( Shift );
		}else
			return RShift_Logic< Shift >();
	}

	template <typename IT>
	SimdTInt256<IT>  SimdTInt256<IT>::RShift_Arith (uint shift)  C_NE___
	{
		if constexpr( IsSigned<IT> )
		{
			StaticAssert( Has_ScalarShift_Arithmetic() );
			ASSERT_MSG( shift <= CT_SizeOfInBits<Scalar_t>, "Result will be zero" );

			if constexpr( is16 )	return Self{ _mm256_srai_epi16( _value, shift )};
			if constexpr( is32 )	return Self{ _mm256_srai_epi32( _value, shift )};
		  #if AE_SIMD_AVX >= 31  // AVX512VL
			if constexpr( is64 )	return Self{ _mm256_srai_epi64( _value, shift )};
		  #endif
		}else
			return RShift_Logic( shift );
	}

	template <typename IT>
	SimdTInt256<IT>  SimdTInt256<IT>::RShift_Arith (const Shift64_t &shift)  C_NE___
	{
		if constexpr( IsSigned<IT> )
		{
			StaticAssert( Has_ScalarShift_Arithmetic() );
			ASSERT_MSG( shift.get<0>() >= 0, "Result will be zero" );
			ASSERT_MSG( shift.get<0>() <= int(CT_SizeOfInBits<Scalar_t>), "Result will be zero" );

			if constexpr( is16 )	return Self{ _mm256_sra_epi16( _value, shift.Ref() )};
			if constexpr( is32 )	return Self{ _mm256_sra_epi32( _value, shift.Ref() )};
		  #if AE_SIMD_AVX >= 31  // AVX512VL
			if constexpr( is64 )	return Self{ _mm256_sra_epi64( _value, shift.Ref() )};
		  #endif
		}else
			return RShift_Logic( shift );
	}

	template <typename IT>
	__Ce__ bool  SimdTInt256<IT>::Has_ScalarShift_Arithmetic ()
	{
		return	is16 or is32 or (is64 and AE_SIMD_AVX >= 31) or
				(IsUnsigned<IT> and Has_ScalarShift_Logic());
	}

/*
=================================================
	LShift_LogicV
----
	Shift elements for individual offset.
	Returns zero element on overflow.
=================================================
*/
	template <typename IT>
	SimdTInt256<IT>  SimdTInt256<IT>::LShift_LogicV (const Unsigned_t &shift)  C_NE___
	{
		StaticAssert( Has_VecLShift_Logic() );
		if constexpr( Unsigned_t::Has_Greater() ) {
			ASSERT_MSG( shift.GEqual( Unsigned_t{} ).All(), "Result will be zero" );
		}

	  #if AE_SIMD_AVX >= 31  // AVX512BW
		if constexpr( is16 )	return Self{ _mm256_sllv_epi16( _value, shift.Ref() )};
	  #endif
		if constexpr( is32 )	return Self{ _mm256_sllv_epi32( _value, shift.Ref() )};
		if constexpr( is64 )	return Self{ _mm256_sllv_epi64( _value, shift.Ref() )};
	}

	template <typename IT>
	__Ce__ bool  SimdTInt256<IT>::Has_VecLShift_Logic ()
	{
	  #if AE_SIMD_AVX >= 31  // AVX512BW
		if constexpr( is16 )	return true;
	  #endif
		if constexpr( is32 )	return true;
		if constexpr( is64 )	return true;
		return false;
	}

/*
=================================================
	RShift_LogicV
----
	Shift elements for individual offset.
	Returns zero element on overflow.
=================================================
*/
	template <typename IT>
	SimdTInt256<IT>  SimdTInt256<IT>::RShift_LogicV (const Unsigned_t &shift)  C_NE___
	{
		StaticAssert( Has_VecRShift_Logic() );
		if constexpr( Unsigned_t::Has_Greater() ) {
			ASSERT_MSG( shift.GEqual( Unsigned_t{} ).All(), "Result will be zero" );
		}

	  #if AE_SIMD_AVX >= 31  // AVX512BW
		if constexpr( is16 )	return Self{ _mm256_srlv_epi16( _value, shift.Ref() )};
	  #endif
		if constexpr( is32 )	return Self{ _mm256_srlv_epi32( _value, shift.Ref() )};
		if constexpr( is64 )	return Self{ _mm256_srlv_epi64( _value, shift.Ref() )};
	}

	template <typename IT>
	__Ce__ bool  SimdTInt256<IT>::Has_VecRShift_Logic ()
	{
	  #if AE_SIMD_AVX >= 31  // AVX512BW
		if constexpr( is16 )	return true;
	  #endif
		if constexpr( is32 )	return true;
		if constexpr( is64 )	return true;

		return false;
	}

/*
=================================================
	RShift_ArithV
----
	Shift for individual offset.
	Returns:
	 - max element on overflow
	 - -1 for negative shift on negative value.
	Used for negative values: '-800 >> 2 = -200'
=================================================
*/
	template <typename IT>
	SimdTInt256<IT>  SimdTInt256<IT>::RShift_ArithV (const Unsigned_t &shift)  C_NE___
	{
		StaticAssert( Has_VecRShift_Arithmetic() );
		if constexpr( IsSigned<IT> )
		{
			if constexpr( Unsigned_t::Has_Greater() ) {
				ASSERT_MSG( shift.GEqual( Unsigned_t{} ).All(), "Result will be zero" );
			}

		  #if AE_SIMD_AVX >= 31  // AVX512BW
			if constexpr( is16 )	return Self{ _mm256_srav_epi16( _value, shift.Ref() )};
			if constexpr( is64 )	return Self{ _mm256_srav_epi64( _value, shift.Ref() )};
		  #endif
			if constexpr( is32 )	return Self{ _mm256_srav_epi32( _value, shift.Ref() )};
		}else
			return RShift_LogicV( shift );
	}

	template <typename IT>
	__Ce__ bool  SimdTInt256<IT>::Has_VecRShift_Arithmetic ()
	{
		if constexpr( IsSigned<IT> )
		{
		  #if AE_SIMD_AVX >= 31  // AVX512BW
			if constexpr( is16 )	return true;
			if constexpr( is64 )	return true;
		  #endif
			if constexpr( is32 )	return true;
			return false;
		}else
			return Has_VecRShift_Logic();
	}

/*
=================================================
	LShift
=================================================
*/
	template <typename IT>
	template <typename T>
	SimdTInt256<IT>  SimdTInt256<IT>::LShift (const T &shift)  C_NE___
	{
		if constexpr( IsSigned<IT> )
		{
			if constexpr( IsSame< T, Unsigned_t >)
				return LShift_ArithV( shift );
			else
				return LShift_Arith( shift );
		}
		else
		{
			if constexpr( IsSame< T, Unsigned_t >)
				return LShift_LogicV( shift );
			else
				return LShift_Logic( shift );
		}
	}

/*
=================================================
	RShift
=================================================
*/
	template <typename IT>
	template <typename T>
	SimdTInt256<IT>  SimdTInt256<IT>::RShift (const T &shift)  C_NE___
	{
		if constexpr( IsSigned<IT> )
		{
			if constexpr( IsSame< T, Unsigned_t >)
				return RShift_ArithV( shift );
			else
				return RShift_Arith( shift );
		}
		else
		{
			if constexpr( IsSame< T, Unsigned_t >)
				return RShift_LogicV( shift );
			else
				return RShift_Logic( shift );
		}
	}

/*
=================================================
	Swizzle (Long4)
=================================================
*/
	template <typename IT>
	template <uint X, uint Y, uint Z, uint W,  typename T> requires( sizeof(T)==8 )
	SimdTInt256<IT>  SimdTInt256<IT>::Swizzle ()  C_NE___
	{
		StaticAssert( Has_Swizzle() );
		StaticAssert( X < count );
		StaticAssert( Y < count );
		StaticAssert( Z < count );
		StaticAssert( W < count );

		// TODO: optimize
		auto	v = ToArray();
		return Self{ v[X], v[Y], v[Z], v[W] };
	}

/*
=================================================
	Shuffle (Int8)
=================================================
*/
	template <typename IT>
	template <uint V0, uint V1, uint V2, uint V3, uint V4, uint V5, uint V6, uint V7, typename T> requires( sizeof(T)==4 )
	SimdTInt256<IT>  SimdTInt256<IT>::Shuffle (const Self &) C_NE___
	{
		StaticAssert( Has_Shuffle() );
		StaticAssert( V0 < count*2 );
		StaticAssert( V1 < count*2 );
		StaticAssert( V2 < count*2 );
		StaticAssert( V3 < count*2 );
		StaticAssert( V4 < count*2 );
		StaticAssert( V5 < count*2 );
		StaticAssert( V6 < count*2 );
		StaticAssert( V7 < count*2 );

		// TODO
	}

/*
=================================================
	Shuffle (Long4)
=================================================
*/
	template <typename IT>
	template <uint X, uint Y, uint Z, uint W,  typename T> requires( sizeof(T)==8 )
	SimdTInt256<IT>  SimdTInt256<IT>::Shuffle (const Self &) C_NE___
	{
		StaticAssert( Has_Shuffle() );
		StaticAssert( X < count*2 );
		StaticAssert( Y < count*2 );
		StaticAssert( Z < count*2 );
		StaticAssert( W < count*2 );

		// TODO
	}

/*
=================================================
	SwizzleLanes
=================================================
*/
	template <typename IT>
	template <uint Low, uint High>
	SimdTInt256<IT>  SimdTInt256<IT>::SwizzleLanes () C_NE___
	{
		StaticAssert( Low  < lanes );
		StaticAssert( High < lanes );
		return Self{_mm256_permute2x128_si256( _value, _value, Low | (High << 4) )};
	}

/*
=================================================
	ToShort
=================================================
*/
	template <typename IT>
	template <uint Idx>
	auto  SimdTInt256<IT>::ToShort () C_NE___
	{
		StaticAssert( sizeof(Scalar_t) != sizeof(ushort) );

		if constexpr( is8 )
		{
			StaticAssert( Idx < 2 );
			const auto	lane = Lane<Idx>()._value;
			if constexpr( isI8 )	return SimdShort16{ _mm256_cvtepi8_epi16( lane )};
			else					return SimdUShort16{_mm256_cvtepu8_epi16( lane )};
		}
	}

/*
=================================================
	ToInt
=================================================
*/
	template <typename IT>
	template <uint Idx>
	auto  SimdTInt256<IT>::ToInt () C_NE___
	{
		StaticAssert( sizeof(Scalar_t) != sizeof(uint) );

		if constexpr( is8 )
		{
			StaticAssert( Idx < 4 );
			const auto	lane = Lane<Idx/2>()._value;
			if constexpr( (Idx & 1) == 0 ){
				if constexpr( isI8 )	return SimdInt8{ _mm256_cvtepi8_epi32( lane )};
				else					return SimdUInt8{_mm256_cvtepu8_epi32( lane )};
			}else{
				auto	high = _mm_shuffle_epi32( lane, _MM_SHUFFLE( 3, 2, 3, 2 ));
				if constexpr( isI8 )	return SimdInt8{ _mm256_cvtepi8_epi32( high )};
				else					return SimdUInt8{_mm256_cvtepu8_epi32( high )};
			}
		}

		if constexpr( is16 )
		{
			StaticAssert( Idx < 2 );
			const auto	lane = Lane<Idx>()._value;
			if constexpr( isI16 )	return SimdInt8{ _mm256_cvtepi16_epi32( lane )};
			else					return SimdUInt8{_mm256_cvtepu16_epi32( lane )};
		}
	}

/*
=================================================
	ToLong
=================================================
*/
	template <typename IT>
	template <uint Idx>
	auto  SimdTInt256<IT>::ToLong () C_NE___
	{
		StaticAssert( sizeof(Scalar_t) != sizeof(ulong) );

		if constexpr( is8 )
		{
			StaticAssert( Idx < 8 );
			const auto	lane = Lane<Idx/4>()._value;
			if constexpr( Idx == 0 ){
				if constexpr( isI8 )	return SimdLong4{ _mm256_cvtepi8_epi64( lane )};
				else					return SimdULong4{_mm256_cvtepu8_epi64( lane )};
			}else{
				auto	p = _mm_shuffle_epi32( lane, _MM_SHUFFLE( 0, 0, 0, Idx&3 ));
				if constexpr( isI8 )	return SimdLong4{ _mm256_cvtepi8_epi64( p )};
				else					return SimdULong4{_mm256_cvtepu8_epi64( p )};
			}
		}

		if constexpr( is16 )
		{
			StaticAssert( Idx < 4 );
			const auto	lane = Lane<Idx/2>()._value;
			if constexpr( (Idx & 1) == 0 ){
				if constexpr( isI16 )	return SimdLong4{ _mm256_cvtepi16_epi64( lane )};
				else					return SimdULong4{_mm256_cvtepu16_epi64( lane )};
			}else{
				auto	high = _mm_shuffle_epi32( lane, _MM_SHUFFLE( 3, 2, 3, 2 ));
				if constexpr( isI16 )	return SimdLong4{ _mm256_cvtepi16_epi64( high )};
				else					return SimdULong4{_mm256_cvtepu16_epi64( high )};
			}
		}

		if constexpr( is32 )
		{
			StaticAssert( Idx < 2 );
			const auto	lane = Lane<Idx>()._value;
			if constexpr( isI32 )	return SimdLong4{ _mm256_cvtepi32_epi64( lane )};
			else					return SimdULong4{_mm256_cvtepu32_epi64( lane )};
		}
	}

/*
=================================================
	PrefixSum
=================================================
*/
	template <typename IT>
	SimdTInt256<IT>  SimdTInt256<IT>::Sum () C_NE___
	{
		StaticAssert( Has_PrefixSum() );

		if constexpr( is64 )
		{
			auto	a = this->Add( this->Swizzle<1,0,3,2>() );
			return a.Add( a.template Swizzle<2,3,0,1>() );
		}
	}

	template <typename IT>
	IT  SimdTInt256<IT>::PrefixSum () C_NE___
	{
		return Sum().template get<0>();
	}

	template <typename IT>
	__Ce__ bool  SimdTInt256<IT>::Has_PrefixSum ()
	{
		if constexpr( is64 )
			return true;
		else
			return false;
	}

/*
=================================================
	PrefixSumExt
=================================================
*/
	template <typename IT>
	auto  SimdTInt256<IT>::SumExt () C_NE___
	{
		StaticAssert( Has_PrefixSumExt() );

		/*if constexpr( is8 )
		{
			auto	a0 = this->ToShort<0>();
			auto	a1 = this->ToShort<1>();
			auto	b  = a0.Add( a1 );
			return b.Sum();
		}
		if constexpr( is16 )
		{
			auto	a0 = this->ToInt<0>();
			auto	a1 = this->ToInt<1>();
			auto	b  = a0.Add( a1 );
			return b.Sum();
		}*/
		if constexpr( is32 )
		{
			auto	a0 = this->ToLong<0>();
			auto	a1 = this->ToLong<1>();
			auto	b  = a0.Add( a1 );
			return b.Sum();
		}
	}

	template <typename IT>
	auto  SimdTInt256<IT>::PrefixSumExt () C_NE___
	{
		return SumExt().template get<0>();
	}

	template <typename IT>
	__Ce__ bool  SimdTInt256<IT>::Has_PrefixSumExt ()
	{
		if constexpr( is32 )
			return true;
		else
			return false;
	}

/*
=================================================
	PrefixMax
=================================================
*/
	template <typename IT>
	SimdTInt256<IT>  SimdTInt256<IT>::Max () C_NE___
	{
		StaticAssert( Has_PrefixMinMax() );

		if constexpr( is64 )
		{
			auto	a = this->Max( this->Swizzle<1,0,3,2>() );
			return a.Max( a.template Swizzle<2,3,0,1>() );
		}
	}

	template <typename IT>
	IT  SimdTInt256<IT>::PrefixMax () C_NE___
	{
		return Max().template get<0>();
	}

/*
=================================================
	PrefixMin
=================================================
*/
	template <typename IT>
	SimdTInt256<IT>  SimdTInt256<IT>::Min () C_NE___
	{
		StaticAssert( Has_PrefixMinMax() );

		if constexpr( is64 )
		{
			auto	a = this->Min( this->Swizzle<1,0,3,2>() );	// min(1,0), min(0,1), min(3,2), min(2,3)
			return a.Min( a.template Swizzle<2,3,0,1>() );		// min(1,0,3,2)
		}
	}

	template <typename IT>
	IT  SimdTInt256<IT>::PrefixMin () C_NE___
	{
		return Min().template get<0>();
	}

/*
=================================================
	Has_PrefixMinMax
=================================================
*/
	template <typename IT>
	__Ce__ bool  SimdTInt256<IT>::Has_PrefixMinMax ()
	{
		if constexpr( is64 )
			return Has_MinMax();
		else
			return false;
	}

# else // not AVX2:

	template <typename IT>	__Ce__ bool  SimdTInt256<IT>::Has_Mul ()					{ return false; }
	template <typename IT>	__Ce__ bool  SimdTInt256<IT>::Has_MinMax ()					{ return false; }
	template <typename IT>	__Ce__ bool  SimdTInt256<IT>::Has_Equal ()					{ return false; }
	template <typename IT>	__Ce__ bool  SimdTInt256<IT>::Has_Greater ()				{ return false; }
	template <typename IT>	__Ce__ bool  SimdTInt256<IT>::Has_ScalarShift_Logic ()		{ return false; }
	template <typename IT>	__Ce__ bool  SimdTInt256<IT>::Has_ScalarShift_Arithmetic ()	{ return false; }
	template <typename IT>	__Ce__ bool  SimdTInt256<IT>::Has_VecLShift_Logic ()		{ return false; }
	template <typename IT>	__Ce__ bool  SimdTInt256<IT>::Has_VecRShift_Logic ()		{ return false; }
	template <typename IT>	__Ce__ bool  SimdTInt256<IT>::Has_VecRShift_Arithmetic ()	{ return false; }
	template <typename IT>	__Ce__ bool  SimdTInt256<IT>::Has_PrefixSum ()				{ return false; }
	template <typename IT>	__Ce__ bool  SimdTInt256<IT>::Has_PrefixSumExt ()			{ return false; }
	template <typename IT>	__Ce__ bool  SimdTInt256<IT>::Has_PrefixMinMax ()			{ return false; }

# endif // AVX2
#endif // AE_SIMD_SimdTInt256
//-----------------------------------------------------------------------------



#ifdef AE_SIMD_Int256b
/*
=================================================
	ToArray
=================================================
*/
	template <typename T>
	auto  Int256b::ToArray ()  C_NE___
	{
		StaticAssert( IsInteger<T> );
		StaticArray< T, sizeof(Native_t) / sizeof(T) >	arr;
		_mm256_storeu_si256( OUT reinterpret_cast<Native_t *>( arr.data() ), _value );
		return arr;
	}

#endif // AE_SIMD_Int256b
//-----------------------------------------------------------------------------

} // AE::Base
