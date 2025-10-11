// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#pragma once

#include "base/SIMD/SSE128.h"

namespace AE::Base
{

#ifdef AE_SIMD_SimdFloat4
/*
=================================================
	Bool4 ctor
=================================================
*/
	inline SimdFloat4::Bool4::Bool4 (const SimdInt4 &v)		__NE___	: Bool4{ v.Ref() } {}
	inline SimdFloat4::Bool4::Bool4 (const SimdUInt4 &v)	__NE___	: Bool4{ v.Ref() } {}

	inline SimdFloat4::Bool4::Bool4 (bool v0, bool v1, bool v2, bool v3) __NE___ : Bool4{ SimdUInt_t{ v0, v1, v2, v3 }} {}

/*
=================================================
	get
=================================================
*/
	template <uint I>
	SimdFloat4::Scalar_t  SimdFloat4::get ()  C_NE___
	{
		StaticAssert( I < count );
	  #if 1
		if constexpr( I == 0 )
			return _mm_cvtss_f32( _value );
		else{
		  #if AE_SIMD_AVX >= 1
			return _mm_cvtss_f32( _mm_permute_ps( _value, _MM_SHUFFLE(I,I,I,I) ));
		  #else
			return _mm_cvtss_f32( _mm_shuffle_ps( _value, _value, _MM_SHUFFLE(I,I,I,I) ));
		  #endif
		}
	  #else
		float	dst;
		_MM_EXTRACT_FLOAT( dst, _value, I );	// SSE4.1
		return dst;
	  #endif
	}

/*
=================================================
	set
=================================================
*/
	template <uint I>
	SimdFloat4  SimdFloat4::set (Scalar_t newValue)  C_NE___
	{
		StaticAssert( I < count );
	  #if AE_SIMD_SSE >= 41
		return Self{_mm_insert_ps( _value, _mm_set_ss(newValue), _MM_MK_INSERTPS_NDX( 0, I, 0 ) )};
	  #else
		return Select( Bool4{I==0, I==1, I==2, I==3}, Self{newValue}, *this );
	  #endif
	}

/*
=================================================
	BitCast
=================================================
*/
	template <typename DstType>
	DstType  SimdFloat4::BitCast ()  C_NE___
	{
		StaticAssert( sizeof(Self) == sizeof(DstType) );
		StaticAssert( Has_BitCast<DstType>() );

		if constexpr( IsSame< DstType, Int128b >)
			return DstType{ _mm_castps_si128( _value )};

		if constexpr( IsSame< DstType, SimdUInt4 > or IsSame< DstType, SimdInt4 >)
			return DstType{ _mm_castps_si128( _value )};
	}

/*
=================================================
	Has_BitCast
=================================================
*/
	template <typename DstType>
	__Ce__ bool  SimdFloat4::Has_BitCast ()
	{
		if constexpr( IsSame< DstType, Int128b >)
			return true;
		else
		if constexpr( IsSame< DstType, SimdUInt4 > or IsSame< DstType, SimdInt4 >)
			return true;
		else
			return false;
	}

/*
=================================================
	ToDouble / ToInt / ToHalf / ToDouble4
=================================================
*/
	template <uint Idx>
	SimdDouble2  SimdFloat4::ToDouble () C_NE___
	{
		StaticAssert( Idx < 2 );
		if constexpr( Idx == 0 )
			return SimdDouble2{_mm_cvtps_pd( _value )};
		else
			return SimdDouble2{_mm_cvtps_pd( _mm_movehl_ps( _value, _value ))};
	}

	inline SimdInt4  SimdFloat4::ToInt () C_NE___
	{
		return SimdInt4{_mm_cvtps_epi32( _value )};
	}

# ifdef AE_SIMD_SimdHalf8
	inline SimdHalf8  SimdFloat4::ToHalf () C_NE___
	{
		return SimdHalf8{ *this };
	}
# endif
# ifdef AE_SIMD_SimdDouble4
	inline SimdDouble4  SimdFloat4::ToDouble4 () C_NE___
	{
		return SimdDouble4{_mm256_cvtps_pd( _value )};
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
	auto  SimdFloat4::Convert ()  C_NE___
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
		#if AE_SIMD_AVX >= 1
			return ToDouble4();
		#else
			return std::array{ ToDouble<0>(), ToDouble<1>() };
		#endif
		}

		if constexpr( IsSame< DstScalar, int >)
			return ToInt();
		else
		if constexpr( IsInteger< DstScalar >)
			return ToInt().Convert< DstScalar >();
	}

/*
=================================================
	Has_Convert
=================================================
*/
	template <typename DstScalar>
	__Ce__ bool  SimdFloat4::Has_Convert ()
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
		if constexpr( IsSame< DstScalar, int >)
			return true;
		else
		if constexpr( IsInteger< DstScalar >)
			return SimdInt4::Has_Convert<DstScalar>();
		else
			return false;
	}

/*
=================================================
	Swizzle
=================================================
*/
	template <uint X, uint Y, uint Z, uint W>
	SimdFloat4  SimdFloat4::Swizzle ()  C_NE___
	{
		StaticAssert( Has_Swizzle() );
		StaticAssert( X < count );
		StaticAssert( Y < count );
		StaticAssert( Z < count );
		StaticAssert( W < count );
	  #if AE_SIMD_AVX >= 1
		return Self{_mm_permute_ps( _value, _MM_SHUFFLE( W, Z, Y, X ) )};
	  #else
		return Self{ _mm_shuffle_ps( _value, _value, _MM_SHUFFLE( W, Z, Y, X ) )};
	  #endif
	}

/*
=================================================
	Shuffle
=================================================
*/
	template <uint X, uint Y, uint Z, uint W>
	SimdFloat4  SimdFloat4::Shuffle (const Self &b)  C_NE___
	{
		StaticAssert( Has_Shuffle() );
		StaticAssert( X < count*2 );
		StaticAssert( Y < count*2 );
		StaticAssert( Z < count*2 );
		StaticAssert( W < count*2 );

		if constexpr( X<count and Y<count and Z>=count and W>=count )
			return Self{ _mm_shuffle_ps( _value, b._value, _MM_SHUFFLE( W-count, Z-count, Y, X ) )};

		if constexpr( Z<count and W<count and X>=count and Y>=count )
			return Self{ _mm_shuffle_ps( b._value, _value, _MM_SHUFFLE( Y-count, X-count, W, Z ) )};
	}

/*
=================================================
	PrefixSum
=================================================
*/
	inline SimdFloat4  SimdFloat4::Sum ()  C_NE___
	{
		StaticAssert( Has_PrefixSum() );
		auto	a = this->Add( this->Swizzle<1,0,3,2>() );
		return a.Add( a.Swizzle<2,3,0,1>() );
	}

	inline float  SimdFloat4::PrefixSum ()  C_NE___
	{
		return Sum().get<0>();
	}

/*
=================================================
	PrefixMax
=================================================
*/
	inline SimdFloat4  SimdFloat4::Max ()  C_NE___
	{
		StaticAssert( Has_PrefixMinMax() );
		auto	a = this->Max( this->Swizzle<1,0,3,2>() );
		return a.Max( a.Swizzle<2,3,0,1>() );
	}

	inline float  SimdFloat4::PrefixMax ()  C_NE___
	{
		return Max().get<0>();
	}

/*
=================================================
	PrefixMin
=================================================
*/
	inline SimdFloat4  SimdFloat4::Min ()  C_NE___
	{
		StaticAssert( Has_PrefixMinMax() );
		auto	a = this->Min( this->Swizzle<1,0,3,2>() );	// min(1,0), min(0,1), min(3,2), min(2,3)
		return a.Min( a.Swizzle<2,3,0,1>() );				// min(1,0,3,2)
	}

	inline float  SimdFloat4::PrefixMin ()  C_NE___
	{
		return Min().get<0>();
	}

/*
=================================================
	Select
=================================================
*/
	Nd__In SimdFloat4  Select (const SimdFloat4::Bool4 &condition, const SimdFloat4 &ifTrue, const SimdFloat4 &ifFalse) __NE___
	{
	  #if AE_SIMD_SSE >= 41
		return SimdFloat4{ _mm_blendv_ps( ifFalse._value, ifTrue._value, condition.Ref() )};
	  #else
		auto	a = _mm_andnot_ps( condition.Ref(), ifFalse._value );
		auto	b = _mm_and_ps( condition.Ref(), ifTrue._value );
		return SimdFloat4{ _mm_or_ps( a, b )};
	  #endif
	}

#endif // AE_SIMD_SimdFloat4
//-----------------------------------------------------------------------------



#ifdef AE_SIMD_SimdDouble2
/*
=================================================
	Bool2 ctor
=================================================
*/
	inline SimdDouble2::Bool2::Bool2 (const SimdInt_t &v)	__NE___	: Bool2{ v.Ref() } {}
	inline SimdDouble2::Bool2::Bool2 (const SimdUInt_t &v)	__NE___	: Bool2{ v.Ref() } {}

	inline SimdDouble2::Bool2::Bool2 (bool v0, bool v1) __NE___ : Bool2{ SimdUInt_t{ v0, v1 }} {}

/*
=================================================
	get
=================================================
*/
	template <uint I>
	SimdDouble2::Scalar_t  SimdDouble2::get ()  C_NE___
	{
		StaticAssert( I < count );
		if constexpr( I == 0 )
			return _mm_cvtsd_f64( _value );
		else{
		  #if AE_SIMD_AVX >= 1
			return _mm_cvtsd_f64( _mm_permute_pd( _value, 1 ));
		  #else
			return _mm_cvtsd_f64( _mm_shuffle_pd( _value, _value, 1 ));
		  #endif
		}
	}

/*
=================================================
	set
=================================================
*/
	template <uint I>
	SimdDouble2  SimdDouble2::set (Scalar_t newValue)  C_NE___
	{
		StaticAssert( I < count );
		if constexpr( I == 0 )
			return Self{ _mm_shuffle_pd( _mm_set_sd(newValue), _value, _MM_SHUFFLE2( 1, 0 ) )};
		else
			return Self{ _mm_shuffle_pd( _value, _mm_set_sd(newValue), _MM_SHUFFLE2( 0, 0 ) )};
	}

/*
=================================================
	BitCast
=================================================
*/
	template <typename DstType>
	DstType  SimdDouble2::BitCast ()  C_NE___
	{
		StaticAssert( sizeof(Self) == sizeof(DstType) );
		StaticAssert( Has_BitCast<DstType>() );

		if constexpr( IsSame< DstType, Int128b >)
			return DstType{ _mm_castpd_si128( _value )};

		if constexpr( IsSame< DstType, SimdULong2 > or IsSame< DstType, SimdLong2 >)
			return DstType{ _mm_castpd_si128( _value )};
	}

/*
=================================================
	Has_BitCast
=================================================
*/
	template <typename DstType>
	__Ce__ bool  SimdDouble2::Has_BitCast ()
	{
		if constexpr( IsSame< DstType, Int128b >)
			return true;
		else
		if constexpr( IsSame< DstType, SimdULong2 > or IsSame< DstType, SimdLong2 >)
			return true;
		else
			return false;
	}

/*
=================================================
	ToFloat / ToLong / ToInt
=================================================
*/
	inline SimdFloat4  SimdDouble2::ToFloat () C_NE___
	{
		return SimdFloat4{_mm_cvtpd_ps( _value )};
	}

	inline SimdInt4  SimdDouble2::ToInt () C_NE___
	{
		return SimdInt4{_mm_cvtpd_epi32( _value )};
	}

# if AE_SIMD_AVX >= 31 // AVX512DQ, AVX512VL
	inline SimdLong2  SimdDouble2::ToLong () C_NE___
	{
		return SimdLong2{_mm_cvtpd_epi64( _value )};
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
	auto  SimdDouble2::Convert ()  C_NE___
	{
		StaticAssert( IsAnyScalar< DstScalar >);
		StaticAssert( not IsSame< DstScalar, Scalar_t >);
		StaticAssert( Has_Convert< DstScalar >() );

	  #ifdef AE_SIMD_SimdHalf8
		if constexpr( IsSame< DstScalar, half >)
			return ToFloat().ToHalf();
		else
	  #endif
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
	__Ce__ bool  SimdDouble2::Has_Convert ()
	{
		if constexpr( IsSame< DstScalar, Scalar_t >)
			return false;
		else
	  #ifdef AE_SIMD_SimdHalf8
		if constexpr( IsSame< DstScalar, half >)
			return true;
		else
	  #endif
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
	Swizzle
=================================================
*/
	template <uint X, uint Y>
	SimdDouble2  SimdDouble2::Swizzle ()  C_NE___
	{
		StaticAssert( Has_Swizzle() );
		StaticAssert( X < count );
		StaticAssert( Y < count );
	  #if AE_SIMD_AVX >= 1
		return Self{_mm_permute_pd( _value, _MM_SHUFFLE2( Y, X ) )};
	  #else
		return Self{ _mm_shuffle_pd( _value, _value, _MM_SHUFFLE2( Y, X ) )};
	  #endif
	}

/*
=================================================
	Shuffle
=================================================
*/
	template <uint X, uint Y>
	SimdDouble2  SimdDouble2::Shuffle (const Self &b)  C_NE___
	{
		StaticAssert( Has_Shuffle() );
		StaticAssert( X < count*2 );
		StaticAssert( Y < count*2 );

		if constexpr( X<count and Y>=count )
			return Self{ _mm_shuffle_pd( _value, b._value, _MM_SHUFFLE2( Y-count, X ))};

		if constexpr( Y<count and X>=count )
			return Self{ _mm_shuffle_pd( b._value, _value, _MM_SHUFFLE2( X-count, Y ))};
	}

/*
=================================================
	Reciprocal_fp32
=================================================
*/
	inline SimdDouble2  SimdDouble2::Reciprocal_fp32 () C_NE___
	{
		__m128	f01 = _mm_cvtpd_ps( _value );
		__m128	rcp = _mm_rcp_ps( f01 );
		return Self{_mm_cvtps_pd( rcp )};
	}

/*
=================================================
	PrefixSum
=================================================
*/
	inline SimdDouble2  SimdDouble2::Sum ()  C_NE___
	{
		StaticAssert( Has_PrefixSum() );
		return Add( Swizzle<1,0>() );
	}

	inline double  SimdDouble2::PrefixSum ()  C_NE___
	{
		return Sum().get<0>();
	}

/*
=================================================
	PrefixMax
=================================================
*/
	inline SimdDouble2  SimdDouble2::Max ()  C_NE___
	{
		StaticAssert( Has_PrefixMinMax() );
		return Max( Swizzle<1,0>() );
	}

	inline double  SimdDouble2::PrefixMax ()  C_NE___
	{
		return Max().get<0>();
	}

/*
=================================================
	PrefixMin
=================================================
*/
	inline SimdDouble2  SimdDouble2::Min ()  C_NE___
	{
		StaticAssert( Has_PrefixMinMax() );
		return Min( Swizzle<1,0>() );
	}

	inline double  SimdDouble2::PrefixMin ()  C_NE___
	{
		return Min().get<0>();
	}

/*
=================================================
	Select
=================================================
*/
	Nd__In SimdDouble2  Select (const SimdDouble2::Bool2 &condition, const SimdDouble2 &ifTrue, const SimdDouble2 &ifFalse) __NE___
	{
	  #if AE_SIMD_SSE >= 41
		return SimdDouble2{ _mm_blendv_pd( ifFalse._value, ifTrue._value, condition.Ref() )};
	  #else
		auto	a = _mm_andnot_pd( condition.Ref(), ifFalse._value );
		auto	b = _mm_and_pd( condition.Ref(), ifTrue._value );
		return SimdDouble2{ _mm_or_pd( a, b )};
	  #endif
	}

#endif // AE_SIMD_SimdDouble2
//-----------------------------------------------------------------------------



#ifdef AE_SIMD_SimdTInt128
/*
=================================================
	constructor
=================================================
*/
	template <typename IT>
	SimdTInt128<IT>::SimdTInt128 (Scalar_t v) __NE___
	{
		if constexpr( is8 )		_value = _mm_set1_epi8(   v );	else
		if constexpr( is16 )	_value = _mm_set1_epi16(  v );	else
		if constexpr( is32 )	_value = _mm_set1_epi32(  v );	else
		if constexpr( is64 )	_value = _mm_set1_epi64x( v );
	}

	template <typename IT>
	template <typename B, typename T> requires( IsSame< T, ubyte > and IsSame< B, bool >)
	SimdTInt128<IT>::SimdTInt128 (B v00, B v01, B v02, B v03, B v04, B v05, B v06, B v07,
								  B v08, B v09, B v10, B v11, B v12, B v13, B v14, B v15) __NE___ :
		SimdTInt128{ T(v00 ? 0xFF : 0), T(v01 ? 0xFF : 0), T(v02 ? 0xFF : 0), T(v03 ? 0xFF : 0),
					 T(v04 ? 0xFF : 0), T(v05 ? 0xFF : 0), T(v06 ? 0xFF : 0), T(v07 ? 0xFF : 0),
					 T(v08 ? 0xFF : 0), T(v09 ? 0xFF : 0), T(v10 ? 0xFF : 0), T(v11 ? 0xFF : 0),
					 T(v12 ? 0xFF : 0), T(v13 ? 0xFF : 0), T(v14 ? 0xFF : 0), T(v15 ? 0xFF : 0) }
	{}

	template <typename IT>
	template <typename B, typename T> requires( IsSame< T, ushort > and IsSame< B, bool >)
	SimdTInt128<IT>::SimdTInt128 (B v0, B v1, B v2, B v3, B v4, B v5, B v6, B v7) __NE___ :
		SimdTInt128{ T(v0 ? 0xFFFF : 0), T(v1 ? 0xFFFF : 0), T(v2 ? 0xFFFF : 0), T(v3 ? 0xFFFF : 0),
					 T(v4 ? 0xFFFF : 0), T(v5 ? 0xFFFF : 0), T(v6 ? 0xFFFF : 0), T(v7 ? 0xFFFF : 0) }
	{}

	template <typename IT>
	template <typename B, typename T> requires( IsSame< T, uint > and IsSame< B, bool >)
	SimdTInt128<IT>::SimdTInt128 (B v0, B v1, B v2, B v3) __NE___ :
		SimdTInt128{ (v0 ? ~0u : 0), (v1 ? ~0u : 0), (v2 ? ~0u : 0), (v3 ? ~0u : 0) }
	{}

	template <typename IT>
	template <typename B, typename T> requires( IsSame< T, ulong > and IsSame< B, bool >)
	SimdTInt128<IT>::SimdTInt128 (B v0, B v1) __NE___ :
		SimdTInt128{ (v0 ? ~0ull : 0), (v1 ? ~0ull : 0) }
	{}

/*
=================================================
	get
=================================================
*/
	template <typename IT>
	template <uint I>
	IT  SimdTInt128<IT>::get ()  C_NE___
	{
		StaticAssert( I < count );

		if constexpr( sizeof(IT) == 2 )
			return IT(_mm_extract_epi16( _value, I ));
		else

	  #if AE_SIMD_SSE >= 41
		if constexpr( sizeof(IT) == 1 )
			return IT(_mm_extract_epi8( _value, I ));
		else
		if constexpr( sizeof(IT) == 4 )
			return IT(_mm_extract_epi32( _value, I ));
		else
	  #endif

		if constexpr( I == 0 )
		{
			if constexpr( sizeof(IT) < 4 )
			{
				uint	u = uint(_mm_cvtsi128_si32( _value ));
				uint	m = uint(~ToUnsignedInteger<IT>{0});
				return IT( u & m );
			}
			if constexpr( is32 )	return IT( _mm_cvtsi128_si32( _value ));
			if constexpr( is64 )	return IT( _mm_cvtsi128_si64( _value ));
		}
		else
		{
			if constexpr( sizeof(IT) < 4 )
			{
				constexpr uint	i = (I * sizeof(IT)) / sizeof(uint);
				constexpr usize	s = (I % (sizeof(uint) / sizeof(IT))) * CT_SizeOfInBits<IT>;

				uint	u = _mm_cvtsi128_si32( _mm_shuffle_epi32( _value, _MM_SHUFFLE(i,i,i,i) ));
				uint	m = uint(~ToUnsignedInteger<IT>{0});
				return IT( (u >> s) & m );
			}
			if constexpr( is32 )	return IT( _mm_cvtsi128_si32( _mm_shuffle_epi32( _value, _MM_SHUFFLE(I,I,I,I) )));
			if constexpr( is64 )	return IT( _mm_cvtsi128_si64( _mm_shuffle_epi32( _value, _MM_SHUFFLE(0,0,3,2) )));
		}
	}

/*
=================================================
	set
=================================================
*/
	template <typename IT>
	template <uint I>
	SimdTInt128<IT>  SimdTInt128<IT>::set (Scalar_t newValue)  C_NE___
	{
		StaticAssert( I < count );

		if constexpr( sizeof(IT) == 2 )
			return Self{_mm_insert_epi16( _value, newValue, I )};
		else

	  #if AE_SIMD_SSE >= 41
		if constexpr( sizeof(IT) == 1 )
			return Self{_mm_insert_epi8( _value, newValue, I )};
		else
		if constexpr( sizeof(IT) == 4 )
			return Self{_mm_insert_epi32( _value, newValue, I )};
		else
	  #endif

		if constexpr( sizeof(IT) == 8 )
			return Select( Unsigned_t{ I==0, I==1 }, Self{newValue}, *this );
		else
		if constexpr( sizeof(IT) == 4 )
			return Select( Unsigned_t{ I==0, I==1, I==2, I==3 }, Self{newValue}, *this );
		else
		if constexpr( sizeof(IT) == 2 )
			return Select( Unsigned_t{ I==0, I==1, I==2, I==3, I==4, I==5, I==6, I==7 }, Self{newValue}, *this );
		else
		if constexpr( sizeof(IT) == 1 )
			return Select( Unsigned_t{	I==0, I==1, I==2,  I==3,  I==4,  I==5,  I==6,  I==7,
										I==8, I==9, I==10, I==11, I==12, I==13, I==14, I==15 }, Self{newValue}, *this );
	}

/*
=================================================
	Add
=================================================
*/
	template <typename IT>
	SimdTInt128<IT>  SimdTInt128<IT>::Add (const Self &rhs)  C_NE___
	{
		if constexpr( is8 )		return Self{ _mm_add_epi8(  _value, rhs._value )};
		if constexpr( is16 )	return Self{ _mm_add_epi16( _value, rhs._value )};
		if constexpr( is32 )	return Self{ _mm_add_epi32( _value, rhs._value )};
		if constexpr( is64 )	return Self{ _mm_add_epi64( _value, rhs._value )};
	}

/*
=================================================
	AddSat
=================================================
*/
	template <typename IT>
	SimdTInt128<IT>  SimdTInt128<IT>::AddSat (const Self &rhs)  C_NE___
	{
		if constexpr( isI8 )	return Self{ _mm_adds_epi8(  _value, rhs._value )};
		if constexpr( isU8 )	return Self{ _mm_adds_epu8(  _value, rhs._value )};
		if constexpr( isI16 )	return Self{ _mm_adds_epi16( _value, rhs._value )};
		if constexpr( isU16 )	return Self{ _mm_adds_epu16( _value, rhs._value )};
	}

/*
=================================================
	Sub
=================================================
*/
	template <typename IT>
	SimdTInt128<IT>  SimdTInt128<IT>::Sub (const Self &rhs)  C_NE___
	{
		if constexpr( is8 )		return Self{ _mm_sub_epi8(  _value, rhs._value )};
		if constexpr( is16 )	return Self{ _mm_sub_epi16( _value, rhs._value )};
		if constexpr( is32 )	return Self{ _mm_sub_epi32( _value, rhs._value )};
		if constexpr( is64 )	return Self{ _mm_sub_epi64( _value, rhs._value )};
	}

/*
=================================================
	SubSat
=================================================
*/
	template <typename IT>
	SimdTInt128<IT>  SimdTInt128<IT>::SubSat (const Self &rhs)  C_NE___
	{
		if constexpr( isI8 )	return Self{ _mm_subs_epi8(  _value, rhs._value )};
		if constexpr( isU8 )	return Self{ _mm_subs_epu8(  _value, rhs._value )};
		if constexpr( isI16 )	return Self{ _mm_subs_epi16( _value, rhs._value )};
		if constexpr( isU16 )	return Self{ _mm_subs_epu16( _value, rhs._value )};
	}

/*
=================================================
	Abs
=================================================
*/
	template <typename IT>
	template <typename T> requires( IsSignedInteger<T> )
	SimdTInt128<IT>  SimdTInt128<IT>::Abs () C_NE___
	{
	  #if AE_SIMD_SSE >= 31
		if constexpr( isI8 )	return Self{ _mm_abs_epi8(  _value )};
		if constexpr( isI16 )	return Self{ _mm_abs_epi16( _value )};
		if constexpr( isI32 )	return Self{ _mm_abs_epi32( _value )};
	  #endif
	  #if AE_SIMD_AVX >= 31  // AVX512VL
		if constexpr( isI64 )	return Self{ _mm_abs_epi64( _value )};
	  #endif
	}

/*
=================================================
	Mul
=================================================
*/
	template <typename IT>
	SimdTInt128<IT>  SimdTInt128<IT>::Mul (const Self &rhs)  C_NE___
	{
		StaticAssert( Has_Mul() );
		if constexpr( isU32 )
		{
		  #if AE_SIMD_SSE >= 41
			return Self{ _mm_mullo_epi32( _value, rhs._value )};
		  #else
			auto	m02 = _mm_mul_epu32( _value, rhs._value );
			auto	m13 = _mm_mul_epu32( _mm_shuffle_epi32( _value,		_MM_SHUFFLE( 3, 3, 1, 1 )),
										 _mm_shuffle_epi32( rhs._value,	_MM_SHUFFLE( 3, 3, 1, 1 )));
			auto	m01 = _mm_unpacklo_epi32( m02, m13 );
			auto	m23 = _mm_unpackhi_epi32( m02, m13 );
			return Self{ _mm_unpacklo_epi64( m01, m23 )};
		  #endif
		}
		if constexpr( isI32 )
		{
		  #if AE_SIMD_SSE >= 41
			return Self{ _mm_mullo_epi32( _value, rhs._value )};
		  #else
			auto	m1 = _mm_mul_epu32( _value, rhs._value );
			auto	m2 = _mm_mul_epu32( _mm_srli_si128( _value, 4 ), _mm_srli_si128( rhs._value, 4 ));
			auto	m  = _mm_unpacklo_epi32( _mm_shuffle_epi32( m1, _MM_SHUFFLE( 0, 0, 2, 0 )),
											 _mm_shuffle_epi32( m2, _MM_SHUFFLE( 0, 0, 2, 0 )) );
			return Self{m};
		  #endif
		}
	}

	template <typename IT>
	__Ce__ bool  SimdTInt128<IT>::Has_Mul ()
	{
		return is32;
	}

/*
=================================================
	MulExt
=================================================
*/
	template <typename IT>
	auto  SimdTInt128<IT>::MulExt (const Self &rhs)  C_NE___
	{
		if constexpr( isU32 )
		{
			auto	m01 = _mm_mul_epu32( _mm_shuffle_epi32( _value,		_MM_SHUFFLE( 1, 1, 0, 0 )),
										 _mm_shuffle_epi32( rhs._value,	_MM_SHUFFLE( 1, 1, 0, 0 )));
			auto	m23 = _mm_mul_epu32( _mm_shuffle_epi32( _value,		_MM_SHUFFLE( 3, 3, 2, 2 )),
										 _mm_shuffle_epi32( rhs._value,	_MM_SHUFFLE( 3, 3, 2, 2 )));
			return std::array{ SimdULong2{m01}, SimdULong2{m23} };
		}

	  #if AE_SIMD_SSE >= 41
		if constexpr( isI32 )
		{
			auto	m01 = _mm_mul_epi32( _mm_shuffle_epi32( _value,		_MM_SHUFFLE( 1, 1, 0, 0 )),
										 _mm_shuffle_epi32( rhs._value,	_MM_SHUFFLE( 1, 1, 0, 0 )));
			auto	m23 = _mm_mul_epi32( _mm_shuffle_epi32( _value,		_MM_SHUFFLE( 3, 3, 2, 2 )),
										 _mm_shuffle_epi32( rhs._value,	_MM_SHUFFLE( 3, 3, 2, 2 )));
			return std::array{ SimdLong2{m01}, SimdLong2{m23} };
		}
	  #endif
	}

	template <typename IT>
	__Ce__ bool  SimdTInt128<IT>::Has_MulExt ()
	{
		return isU32 or (isI32 and (AE_SIMD_SSE >= 41));
	}

/*
=================================================
	Min
=================================================
*/
	template <typename IT>
	SimdTInt128<IT>  SimdTInt128<IT>::Min (const Self &rhs)  C_NE___
	{
		StaticAssert( Has_MinMax() );
		if constexpr( isU8 )	return Self{ _mm_min_epu8(  _value, rhs._value )};	else
		if constexpr( isI16 )	return Self{ _mm_min_epi16( _value, rhs._value )};	else
	  #if AE_SIMD_SSE >= 41
		if constexpr( isI8 )	return Self{ _mm_min_epi8(  _value, rhs._value )};	else
		if constexpr( isU16 )	return Self{ _mm_min_epu16( _value, rhs._value )};	else
		if constexpr( isI32 )	return Self{ _mm_min_epi32( _value, rhs._value )};	else
		if constexpr( isU32 )	return Self{ _mm_min_epu32( _value, rhs._value )};	else
	  #endif
	  #if AE_SIMD_AVX >= 31	// AVX512VL
		if constexpr( isI64 )	return Self{ _mm_min_epi64( _value, rhs._value )};	else
		if constexpr( isU64 )	return Self{ _mm_min_epu64( _value, rhs._value )};	else
	  #endif
		if constexpr( isI32 )
			return Select( Bool_t{_mm_cmpgt_epi32( _value, rhs._value )}, rhs, *this );
	}

	template <typename IT>
	__Ce__ bool  SimdTInt128<IT>::Has_MinMax ()
	{
		return	isU8 or isI16 or
				((isI8 or isU16 or isI32 or isU32) and (AE_SIMD_SSE >= 41)) or
				((isI64 or isU64) and (AE_SIMD_AVX >= 31)) or
				isI32;
	}

/*
=================================================
	Max
=================================================
*/
	template <typename IT>
	SimdTInt128<IT>  SimdTInt128<IT>::Max (const Self &rhs)  C_NE___
	{
		StaticAssert( Has_MinMax() );
		if constexpr( isU8 )	return Self{ _mm_max_epu8(  _value, rhs._value )};	else
		if constexpr( isI16 )	return Self{ _mm_max_epi16( _value, rhs._value )};	else
	  #if AE_SIMD_SSE >= 41
		if constexpr( isI8 )	return Self{ _mm_max_epi8(  _value, rhs._value )};	else
		if constexpr( isU16 )	return Self{ _mm_max_epu16( _value, rhs._value )};	else
		if constexpr( isI32 )	return Self{ _mm_max_epi32( _value, rhs._value )};	else
		if constexpr( isU32 )	return Self{ _mm_max_epu32( _value, rhs._value )};	else
	  #endif
	  #if AE_SIMD_AVX >= 31	// AVX512VL
		if constexpr( isI64 )	return Self{ _mm_max_epi64( _value, rhs._value )};	else
		if constexpr( isU64 )	return Self{ _mm_max_epu64( _value, rhs._value )};	else
	  #endif
		if constexpr( isI32 )
			return Select( Bool_t{_mm_cmpgt_epi32( _value, rhs._value )}, *this, rhs );
	}

/*
=================================================
	PrefixSum
=================================================
*/
	template <typename IT>
	SimdTInt128<IT>  SimdTInt128<IT>::Sum () C_NE___
	{
		StaticAssert( Has_PrefixSum() );

		if constexpr( is32 )
		{
			auto	a = this->Add( this->Swizzle<1,0,3,2>() );
			return a.Add( a.template Swizzle<2,3,0,1>() );
		}
		if constexpr( is64 )
		{
			return Add( Swizzle<1,0>() );
		}
	}

	template <typename IT>
	IT  SimdTInt128<IT>::PrefixSum () C_NE___
	{
		return Sum().template get<0>();
	}

	template <typename IT>
	__Ce__ bool  SimdTInt128<IT>::Has_PrefixSum ()
	{
		if constexpr( is32 or is64 )
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
	auto  SimdTInt128<IT>::SumExt () C_NE___
	{
		StaticAssert( Has_PrefixSumExt() );

		/*if constexpr( is8 )
		{
			auto	a0 = this->ToShort<0>();
			auto	a1 = this->ToShort<1>();
			auto	b  = a0.Add( a1 );
			return b.Sum();
		}*/
	  #if AE_SIMD_SSE >= 41
		if constexpr( is16 )
		{
			auto	a0 = this->ToInt<0>();
			auto	a1 = this->ToInt<1>();
			auto	b  = a0.Add( a1 );
			return b.Sum();
		}

		if constexpr( is32 )
		{
			auto	a0 = this->ToLong<0>();
			auto	a1 = this->ToLong<1>();
			auto	b  = a0.Add( a1 );
			return b.Sum();
		}
	  #endif
	}

	template <typename IT>
	auto  SimdTInt128<IT>::PrefixSumExt () C_NE___
	{
		return SumExt().template get<0>();
	}

	template <typename IT>
	__Ce__ bool  SimdTInt128<IT>::Has_PrefixSumExt ()
	{
	  #if AE_SIMD_SSE >= 41
		if constexpr( is16 or is32 )
			return true;
		else
	  #endif
			return false;
	}

/*
=================================================
	PrefixMax
=================================================
*/
	template <typename IT>
	SimdTInt128<IT>  SimdTInt128<IT>::Max () C_NE___
	{
		StaticAssert( Has_PrefixMinMax() );

		if constexpr( is32 )
		{
			auto	a = this->Max( this->Swizzle<1,0,3,2>() );
			return a.Max( a.template Swizzle<2,3,0,1>() );
		}
		if constexpr( is64 )
		{
			return Max( Swizzle<1,0>() );
		}
	}

	template <typename IT>
	IT  SimdTInt128<IT>::PrefixMax () C_NE___
	{
		return Max().template get<0>();
	}

/*
=================================================
	PrefixMin
=================================================
*/
	template <typename IT>
	SimdTInt128<IT>  SimdTInt128<IT>::Min () C_NE___
	{
		StaticAssert( Has_PrefixMinMax() );

		if constexpr( is32 )
		{
			auto	a = this->Min( this->Swizzle<1,0,3,2>() );
			return a.Min( a.template Swizzle<2,3,0,1>() );
		}
		if constexpr( is64 )
		{
			return Min( Swizzle<1,0>() );
		}
	}

	template <typename IT>
	IT  SimdTInt128<IT>::PrefixMin () C_NE___
	{
		return Min().template get<0>();
	}

/*
=================================================
	Has_PrefixMinMax
=================================================
*/
	template <typename IT>
	__Ce__ bool  SimdTInt128<IT>::Has_PrefixMinMax ()
	{
		if constexpr( is32 or is64 )
			return Has_MinMax();
		else
			return false;
	}

/*
=================================================
	Equal
=================================================
*/
	template <typename IT>
	typename SimdTInt128<IT>::Bool_t  SimdTInt128<IT>::Equal (const Self &rhs)  C_NE___
	{
		StaticAssert( Has_Equal() );
		if constexpr( is8 )		return Bool_t{ _mm_cmpeq_epi8(  _value, rhs._value )};
		if constexpr( is16 )	return Bool_t{ _mm_cmpeq_epi16( _value, rhs._value )};
		if constexpr( is32 )	return Bool_t{ _mm_cmpeq_epi32( _value, rhs._value )};
	  #if AE_SIMD_SSE >= 41
		if constexpr( is64 )	return Bool_t{ _mm_cmpeq_epi64( _value, rhs._value )};
	  #endif
	}

	template <typename IT>
	__Ce__ bool  SimdTInt128<IT>::Has_Equal ()
	{
		return is64 ? (AE_SIMD_SSE >= 41) : true;
	}

/*
=================================================
	Greater
----
	a > b
=================================================
*/
	template <typename IT>
	typename SimdTInt128<IT>::Bool_t  SimdTInt128<IT>::Greater (const Self &rhs)  C_NE___
	{
		StaticAssert( Has_Greater() );

		if constexpr( isI8 )			return Bool_t{ _mm_cmpgt_epi8(  _value, rhs._value )};	else
		if constexpr( isI16 )			return Bool_t{ _mm_cmpgt_epi16( _value, rhs._value )};	else
		if constexpr( isI32 )			return Bool_t{ _mm_cmpgt_epi32( _value, rhs._value )};	else
	  #if AE_SIMD_SSE >= 42
		if constexpr( isI64 )			return Bool_t{ _mm_cmpgt_epi64( _value, rhs._value )};	else
	  #endif
		if constexpr( Has_MinMax() )	return *this != this->Min( rhs );	// not LEqual
	}

	template <typename IT>
	__Ce__ bool  SimdTInt128<IT>::_Has_Native_Greater ()
	{
		return isI8 or isI16 or isI32 or (isI64 and AE_SIMD_SSE >= 42);
	}

	template <typename IT>
	__Ce__ bool  SimdTInt128<IT>::Has_Greater ()
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
	typename SimdTInt128<IT>::Bool_t  SimdTInt128<IT>::LEqual (const Self &rhs)  C_NE___
	{
		StaticAssert( Has_Greater() );

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
	SimdTInt128<IT>  SimdTInt128<IT>::LShift_Logic ()  C_NE___
	{
		StaticAssert( Shift <= CT_SizeOfInBits<Scalar_t> );
		return LShift_Logic( Shift );
	}

	template <typename IT>
	SimdTInt128<IT>  SimdTInt128<IT>::LShift_Logic (uint shift)  C_NE___
	{
		StaticAssert( Has_ScalarShift_Logic() );
		ASSERT_MSG( shift <= CT_SizeOfInBits<Scalar_t>, "Result will be zero" );

		if constexpr( is16 )	return Self{ _mm_slli_epi16( _value, shift )};
		if constexpr( is32 )	return Self{ _mm_slli_epi32( _value, shift )};
		if constexpr( is64 )	return Self{ _mm_slli_epi64( _value, shift )};
	}

	template <typename IT>
	SimdTInt128<IT>  SimdTInt128<IT>::LShift_Logic (const Shift64_t &shift)  C_NE___
	{
		StaticAssert( Has_ScalarShift_Logic() );
		ASSERT_MSG( shift.get<0>() >= 0, "Result will be zero" );
		ASSERT_MSG( shift.get<0>() <= int(CT_SizeOfInBits<Scalar_t>), "Result will be zero" );

		if constexpr( is16 )	return Self{ _mm_sll_epi16( _value, shift.Ref() )};
		if constexpr( is32 )	return Self{ _mm_sll_epi32( _value, shift.Ref() )};
		if constexpr( is64 )	return Self{ _mm_sll_epi64( _value, shift.Ref() )};
	}

	template <typename IT>
	__Ce__ bool  SimdTInt128<IT>::Has_ScalarShift_Logic ()
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
	SimdTInt128<IT>  SimdTInt128<IT>::RShift_Logic ()  C_NE___
	{
		StaticAssert( Has_ScalarShift_Logic() );
		StaticAssert( Shift > 0 );
		StaticAssert( Shift <= CT_SizeOfInBits<Scalar_t> );
		return RShift_Logic( Shift );
	}

	template <typename IT>
	SimdTInt128<IT>  SimdTInt128<IT>::RShift_Logic (uint shift)  C_NE___
	{
		StaticAssert( Has_ScalarShift_Logic() );
		ASSERT_MSG( shift <= CT_SizeOfInBits<Scalar_t>, "Result will be zero" );

		if constexpr( is16 )	return Self{ _mm_srli_epi16( _value, shift )};
		if constexpr( is32 )	return Self{ _mm_srli_epi32( _value, shift )};
		if constexpr( is64 )	return Self{ _mm_srli_epi64( _value, shift )};
	}

	template <typename IT>
	SimdTInt128<IT>  SimdTInt128<IT>::RShift_Logic (const Shift64_t &shift)  C_NE___
	{
		StaticAssert( Has_ScalarShift_Logic() );
		ASSERT_MSG( shift.get<0>() >= 0, "Result will be zero" );
		ASSERT_MSG( shift.get<0>() <= int(CT_SizeOfInBits<Scalar_t>), "Result will be zero" );

		if constexpr( is16 )	return Self{ _mm_srl_epi16( _value, shift.Ref() )};
		if constexpr( is32 )	return Self{ _mm_srl_epi32( _value, shift.Ref() )};
		if constexpr( is64 )	return Self{ _mm_srl_epi64( _value, shift.Ref() )};
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
	SimdTInt128<IT>  SimdTInt128<IT>::RShift_Arith ()  C_NE___
	{
		if constexpr( IsSigned<IT> )
		{
			StaticAssert( Shift <= CT_SizeOfInBits<Scalar_t> );
			return RShift_Arith( Shift );
		}else
			return RShift_Logic< Shift >();
	}

	template <typename IT>
	SimdTInt128<IT>  SimdTInt128<IT>::RShift_Arith (uint shift)  C_NE___
	{
		if constexpr( IsSigned<IT> )
		{
			StaticAssert( Has_ScalarShift_Arithmetic() );
			ASSERT_MSG( shift <= CT_SizeOfInBits<Scalar_t>, "Result will be zero" );

			if constexpr( is16 )	return Self{ _mm_srai_epi16( _value, shift )};
			if constexpr( is32 )	return Self{ _mm_srai_epi32( _value, shift )};
		  #if AE_SIMD_AVX >= 31  // AVX512VL
			if constexpr( is64 )	return Self{ _mm_srai_epi64( _value, shift )};
		  #endif
		}else
			return RShift_Logic( shift );
	}

	template <typename IT>
	SimdTInt128<IT>  SimdTInt128<IT>::RShift_Arith (const Shift64_t &shift)  C_NE___
	{
		if constexpr( IsSigned<IT> )
		{
			StaticAssert( Has_ScalarShift_Arithmetic() );
			ASSERT_MSG( shift.get<0>() >= 0, "Result will be zero" );
			ASSERT_MSG( shift.get<0>() <= int(CT_SizeOfInBits<Scalar_t>), "Result will be zero" );

			if constexpr( is16 )	return Self{ _mm_sra_epi16( _value, shift.Ref() )};
			if constexpr( is32 )	return Self{ _mm_sra_epi32( _value, shift.Ref() )};
		  #if AE_SIMD_AVX >= 31  // AVX512VL
			if constexpr( is64 )	return Self{ _mm_sra_epi64( _value, shift.Ref() )};
		  #endif
		}else
			return RShift_Logic( shift );
	}

	template <typename IT>
	__Ce__ bool  SimdTInt128<IT>::Has_ScalarShift_Arithmetic ()
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
	SimdTInt128<IT>  SimdTInt128<IT>::LShift_LogicV (const Unsigned_t &shift)  C_NE___
	{
		StaticAssert( Has_VecLShift_Logic() );
		if constexpr( Unsigned_t::Has_Greater() ) {
			ASSERT_MSG( shift.GEqual( Unsigned_t{} ).All(), "Result will be zero" );
		}

	  #if AE_SIMD_AVX >= 31  // AVX512BW
		if constexpr( is16 )	return Self{ _mm_sllv_epi16( _value, shift.Ref() )};
	  #endif
	  #if AE_SIMD_AVX >= 2	// AVX2
		if constexpr( is32 )	return Self{ _mm_sllv_epi32( _value, shift.Ref() )};
		if constexpr( is64 )	return Self{ _mm_sllv_epi64( _value, shift.Ref() )};
	  #endif
	}

	template <typename IT>
	__Ce__ bool  SimdTInt128<IT>::Has_VecLShift_Logic ()
	{
	  #if AE_SIMD_AVX >= 31  // AVX512BW
		if constexpr( is16 )	return true;
	  #endif
	  #if AE_SIMD_AVX >= 2	// AVX2
		if constexpr( is32 )	return true;
		if constexpr( is64 )	return true;
	  #endif
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
	SimdTInt128<IT>  SimdTInt128<IT>::RShift_LogicV (const Unsigned_t &shift)  C_NE___
	{
		StaticAssert( Has_VecRShift_Logic() );
		if constexpr( Unsigned_t::Has_Greater() ) {
			ASSERT_MSG( shift.GEqual( Unsigned_t{} ).All(), "Result will be zero" );
		}

	  #if AE_SIMD_AVX >= 31  // AVX512BW
		if constexpr( is16 )	return Self{ _mm_srlv_epi16( _value, shift.Ref() )};
	  #endif
	  #if AE_SIMD_AVX >= 2	// AVX2
		if constexpr( is32 )	return Self{ _mm_srlv_epi32( _value, shift.Ref() )};
		if constexpr( is64 )	return Self{ _mm_srlv_epi64( _value, shift.Ref() )};
	  #endif
	}

	template <typename IT>
	__Ce__ bool  SimdTInt128<IT>::Has_VecRShift_Logic ()
	{
	  #if AE_SIMD_AVX >= 31  // AVX512BW
		if constexpr( is16 )	return true;
	  #endif
	  #if AE_SIMD_AVX >= 2	// AVX2
		if constexpr( is32 )	return true;
		if constexpr( is64 )	return true;
	  #endif
		return false;
	}

/*
=================================================
	RShift_ArithV
----
	Shift elements for individual offset.
	Returns:
	 - max element on overflow
	 - -1 for negative shift on negative value.
	Used for negative values: '-800 >> 2 = -200'
=================================================
*/
	template <typename IT>
	SimdTInt128<IT>  SimdTInt128<IT>::RShift_ArithV (const Unsigned_t &shift)  C_NE___
	{
		StaticAssert( Has_VecRShift_Arithmetic() );
		if constexpr( IsSigned<IT> )
		{
			if constexpr( Unsigned_t::Has_Greater() ) {
				ASSERT_MSG( shift.GEqual( Unsigned_t{} ).All(), "Result will be zero" );
			}

		  #if AE_SIMD_AVX >= 31  // AVX512BW
			if constexpr( is16 )	return Self{ _mm_srav_epi16( _value, shift.Ref() )};
			if constexpr( is64 )	return Self{ _mm_srav_epi64( _value, shift.Ref() )};
		  #endif
		  #if AE_SIMD_AVX >= 2	// AVX2
			if constexpr( is32 )	return Self{ _mm_srav_epi32( _value, shift.Ref() )};
		  #endif
		}else
			return RShift_LogicV( shift );
	}

	template <typename IT>
	__Ce__ bool  SimdTInt128<IT>::Has_VecRShift_Arithmetic ()
	{
		if constexpr( IsSigned<IT> )
		{
		  #if AE_SIMD_AVX >= 31  // AVX512BW
			if constexpr( is16 )	return true;
			if constexpr( is64 )	return true;
		  #endif
		  #if AE_SIMD_AVX >= 2	// AVX2
			if constexpr( is32 )	return true;
		  #endif
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
	SimdTInt128<IT>  SimdTInt128<IT>::LShift (const T &shift)  C_NE___
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
	SimdTInt128<IT>  SimdTInt128<IT>::RShift (const T &shift)  C_NE___
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
	Swizzle (Byte16)
=================================================
*/
	template <typename IT>
	template <uint V0, uint V1, uint V2,  uint V3,  uint V4,  uint V5,  uint V6,  uint V7,
			  uint V8, uint V9, uint V10, uint V11, uint V12, uint V13, uint V14, uint V15, typename T> requires( sizeof(T)==1 )
	SimdTInt128<IT>  SimdTInt128<IT>::Swizzle () C_NE___
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
		StaticAssert( V8 < count );
		StaticAssert( V9 < count );
		StaticAssert( V10 < count );
		StaticAssert( V11 < count );
		StaticAssert( V12 < count );
		StaticAssert( V13 < count );
		StaticAssert( V14 < count );
		StaticAssert( V15 < count );
		
	  #if AE_SIMD_SSE > 31 // SSSE3
		const __m128i shuffle_mask = _mm_setr_epi8(
			sbyte(V0),  sbyte(V1),  sbyte(V2),  sbyte(V3),
			sbyte(V4),  sbyte(V5),  sbyte(V6),  sbyte(V7),
			sbyte(V8),  sbyte(V9),  sbyte(V10), sbyte(V11),
			sbyte(V12), sbyte(V13), sbyte(V14), sbyte(V15)
		);
		return SimdTInt128<IT>{ _mm_shuffle_epi8( _value, shuffle_mask )};
	  #else
		auto	arr = ToArray();
		return SimdTInt128<IT>{ arr[V0], arr[V1], arr[V2], arr[V3], arr[V4], arr[V5], arr[V6], arr[V7],
								arr[V8], arr[V9], arr[V10], arr[V11], arr[V12], arr[V13], arr[V14], arr[V15] };
	  #endif
	}

/*
=================================================
	Swizzle (Short8)
=================================================
*/
	template <typename IT>
	template <uint V0, uint V1, uint V2, uint V3, uint V4, uint V5, uint V6, uint V7, typename T> requires( sizeof(T)==2 )
	SimdTInt128<IT>  SimdTInt128<IT>::Swizzle () C_NE___
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
		if constexpr( is_def_lo and (V4 >= 4 and V5 >= 4 and V6 >= 4 and V7 >= 4) )
		{
			return Self{_mm_shufflehi_epi16( _value, _MM_SHUFFLE( V7-4, V6-4, V5-4, V4-4 ))};
		}else
		if constexpr( is_def_hi and (V0 < 4 and V1 < 4 and V2 < 4 and V3 < 4) )
		{
			return Self{_mm_shufflelo_epi16( _value, _MM_SHUFFLE( V3, V2, V1, V0 ))};
		}else
		if constexpr( IsSame< ReqHi, DefLo > and IsSame< ReqLo, DefHi >)
		{
			return Self{_mm_shuffle_epi32( _value, _MM_SHUFFLE( 1, 0, 3, 2 ))};
		}else
		if constexpr( IsSame< Req, UIntSequence< V0,V0,V0,V0, V0,V0,V0,V0 >>)
		{
		  #if AE_SIMD_AVX >= 2
			if constexpr( V0 == 0 )
				return Self{_mm_broadcastw_epi16( _value )};
			else
			if constexpr( V0 < 4 )
			{
				auto	p = _mm_shufflelo_epi16( _value, _MM_SHUFFLE(V0,V0,V0,V0) );
				return Self{_mm_broadcastw_epi16( p )};
			}else
			{
				constexpr uint	i = V0 / 2;
				constexpr uint	j = V0 & 1;

				auto	a = _mm_shuffle_epi32( _value, _MM_SHUFFLE(i,i,i,i) );
				auto	b = _mm_shufflelo_epi16( a, _MM_SHUFFLE(j,j,j,j) );
				return Self{_mm_broadcastw_epi16( b )};
			}
		  #else
			return Self{_mm_set1_epi16( short(_mm_extract_epi16( _value, V0 )))};
		  #endif
		}else
		if constexpr( IsSame< Req, UIntSequence< 0,4, 1,5, 2,6, 3,7 >>)
		{
			auto	high = _mm_shuffle_epi32( _value, _MM_SHUFFLE( 3, 2, 3, 2 ));
			return Self{_mm_unpacklo_epi16( _value, high )};
		}else
		if constexpr( IsSame< Req, UIntSequence< 0,0, 1,1, 2,2, 3,3 >>)
		{
			auto	low = _mm_shuffle_epi32( _value, _MM_SHUFFLE( 1, 0, 1, 0 ));
			return Self{_mm_unpacklo_epi16( _value, low )};
		}else
		if constexpr( IsSame< Req, UIntSequence< 4,4, 5,5, 6,6, 7,7 >>)
		{
			auto	high = _mm_shuffle_epi32( _value, _MM_SHUFFLE( 3, 2, 3, 2 ));
			return Self{_mm_unpackhi_epi16( _value, high )};
		}else
		if constexpr( V1 == V0+1 and V3 == V2+1 and V5 == V4+1 and V7 == V6+1 and
					  V1&1 and V3&1 and V5&1 and V7&1 )
		{
			// example: 0,1, 4,5, 2,3, 6,7
			return Self{_mm_shuffle_epi32( _value, _MM_SHUFFLE( V6/2, V4/2, V2/2, V0/2 ))};
		}else
		if constexpr( V0 <= 3 and V1 <= 3 and V2 <= 3 and V3 <= 3 and
					  V4 >= 4 and V5 >= 4 and V6 >= 4 and V7 >= 4 )
		{
			auto	a = _mm_shufflelo_epi16( _value, _MM_SHUFFLE( V3, V2, V1, V0 ));
			auto	b = _mm_shufflehi_epi16( a,		 _MM_SHUFFLE( V7-4, V6-4, V5-4, V4-4 ));
			return Self{b};
		}else
		if constexpr( V4 <= 3 and V5 <= 3 and V6 <= 3 and V7 <= 3 and
					  V0 >= 4 and V1 >= 4 and V2 >= 4 and V3 >= 4 )
		{
			auto	a = _mm_shuffle_epi32( _value, _MM_SHUFFLE( 1, 0, 3, 2 ));
			auto	b = _mm_shufflelo_epi16( a, _MM_SHUFFLE( V3-4, V2-4, V1-4, V0-4 ));
			auto	c = _mm_shufflehi_epi16( b, _MM_SHUFFLE( V7, V6, V5, V4 ));
			return Self{c};
		}
	}

/*
=================================================
	Swizzle (Int4)
=================================================
*/
	template <typename IT>
	template <uint X, uint Y, uint Z, uint W,  typename T> requires( sizeof(T)==4 )
	SimdTInt128<IT>  SimdTInt128<IT>::Swizzle () C_NE___
	{
		StaticAssert( Has_Swizzle() );
		StaticAssert( X < count );
		StaticAssert( Y < count );
		StaticAssert( Z < count );
		StaticAssert( W < count );
		return Self{ _mm_shuffle_epi32( _value, _MM_SHUFFLE( W, Z, Y, X ))};
	}

/*
=================================================
	Swizzle (Long2)
=================================================
*/
	template <typename IT>
	template <uint X, uint Y,  typename T> requires( sizeof(T)==8 )
	SimdTInt128<IT>  SimdTInt128<IT>::Swizzle ()  C_NE___
	{
		StaticAssert( Has_Swizzle() );
		StaticAssert( X < count );
		StaticAssert( Y < count );
		const uint	x = X*2;
		const uint	y = Y*2;
		return Self{ _mm_shuffle_epi32( _value, _MM_SHUFFLE( y+1, y+0, x+1, x+0 ))};
	}

/*
=================================================
	Shuffle (Short8)
=================================================
*/
	template <typename IT>
	template <uint V0, uint V1, uint V2, uint V3, uint V4, uint V5, uint V6, uint V7, typename T> requires( sizeof(T)==2 )
	SimdTInt128<IT>  SimdTInt128<IT>::Shuffle (const Self &v8) C_NE___
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

		using Req = UIntSequence< V0, V1, V2, V3, V4, V5, V6, V7 >;

		if constexpr( IsSame< Req, UIntSequence< 0,1,2,3, 8,9,10,11 >>)
			return Self{ _mm_unpacklo_epi64( _value, v8._value )};

		if constexpr( IsSame< Req, UIntSequence< 4,5,6,7, 12,13,14,15 >>)
			return Self{ _mm_unpackhi_epi64( _value, v8._value )};
	}

/*
=================================================
	Shuffle (Int4)
=================================================
*/
	template <typename IT>
	template <uint X, uint Y, uint Z, uint W,  typename T> requires( sizeof(T)==4 )
	SimdTInt128<IT>  SimdTInt128<IT>::Shuffle (const Self &v4567) C_NE___
	{
		StaticAssert( Has_Shuffle() );
		StaticAssert( X < count*2 );
		StaticAssert( Y < count*2 );
		StaticAssert( Z < count*2 );
		StaticAssert( W < count*2 );

		if constexpr( X==0 and Y==1 and Z==4 and W==5 )
			return Self{ _mm_unpacklo_epi64( _value, v4567._value )};

		if constexpr( X==2 and Y==3 and Z==6 and W==7 )
			return Self{ _mm_unpackhi_epi64( _value, v4567._value )};
	}

/*
=================================================
	Shuffle (Long2)
=================================================
*/
	template <typename IT>
	template <uint X, uint Y,  typename T> requires( sizeof(T)==8 )
	SimdTInt128<IT>  SimdTInt128<IT>::Shuffle (const Self &v23) C_NE___
	{
		StaticAssert( Has_Shuffle() );
		StaticAssert( X < count*2 );
		StaticAssert( Y < count*2 );

		if constexpr( X==0 and Y==2 )
			return Self{ _mm_unpacklo_epi64( _value, v23._value )};

		if constexpr( X==1 and Y==3 )
			return Self{ _mm_unpackhi_epi64( _value, v23._value )};
	}

/*
=================================================
	BitCast
=================================================
*/
	template <typename IT>
	template <typename DstType>
	DstType  SimdTInt128<IT>::BitCast ()  C_NE___
	{
		StaticAssert( sizeof(Self) == sizeof(DstType) );
		StaticAssert( Has_BitCast<DstType>() );

		if constexpr( IsSame< DstType, Int128b >)
			return DstType{ _value };

		if constexpr( IsSpecializationOf< DstType, SimdTInt128 >)
			return DstType{ _value };

		if constexpr( IsSame< DstType, SimdDouble2 > and (sizeof(IT) == sizeof(double)) )
			return SimdDouble2{ _mm_castsi128_pd( _value )};

		if constexpr( IsSame< DstType, SimdFloat4 > and (sizeof(IT) == sizeof(float)) )
			return SimdFloat4{ _mm_castsi128_ps( _value )};

	  #ifdef AE_SIMD_SimdHalf8
		if constexpr( IsSame< DstType, SimdHalf8 > and (sizeof(IT) == sizeof(half)) )
			return SimdHalf8{ _value };
	  #endif
	}

/*
=================================================
	Has_BitCast
=================================================
*/
	template <typename IT>
	template <typename DstType>
	__Ce__ bool  SimdTInt128<IT>::Has_BitCast ()
	{
		if constexpr( IsSame< DstType, Int128b >)
			return true;
		else
		if constexpr( IsSpecializationOf< DstType, SimdTInt128 >)
			return true;
		else
		if constexpr( IsSame< DstType, SimdDouble2 > and (sizeof(IT) == sizeof(double)) )
			return true;
		else
		if constexpr( IsSame< DstType, SimdFloat4 > and (sizeof(IT) == sizeof(float)) )
			return true;
		else
	  #ifdef AE_SIMD_SimdHalf8
		if constexpr( IsSame< DstType, SimdHalf8 > and (sizeof(IT) == sizeof(half)) )
			return true;
		else
	  #endif
			return false;
	}

/*
=================================================
	_IntToDouble2
=================================================
*/
	template <typename IT>
	template <uint I, typename T> requires( IsSame<T,int> )
	SimdDouble2  SimdTInt128<IT>::_IntToDouble2 () C_NE___
	{
		StaticAssert( I < 2 );
		if constexpr( I == 0 )
			return SimdDouble2{_mm_cvtepi32_pd( _value )};
		else
			return SimdDouble2{_mm_cvtepi32_pd( Swizzle<2,3,2,3>()._value )};
	}

/*
=================================================
	_UIntToDouble2
=================================================
*/
	template <typename IT>
	template <uint I, typename T> requires( IsSame<T,uint> )
	SimdDouble2  SimdTInt128<IT>::_UIntToDouble2 () C_NE___
	{
		StaticAssert( I < 2 );

	  #if AE_SIMD_AVX >= 30  // AVX512F
		if constexpr( I == 0 )
			return SimdDouble2{_mm_cvtepu32_pd( _value )};
		else
			return SimdDouble2{_mm_cvtepu32_pd( Swizzle<2,3,2,3>()._value )};
	  #else
		auto	v	 = (I == 0 ? _value : Swizzle<2,3,2,3>()._value);
		auto	u_lo = _mm_and_si128( v, _mm_set1_epi32(0xFFFF) );
		auto	u_hi = _mm_srli_epi32( v, 16 );
		auto	d_lo = _mm_cvtepi32_pd( u_lo );
		auto	d_hi = _mm_cvtepi32_pd( u_hi );
				d_hi = _mm_mul_pd( _mm_set1_pd(65536.0), d_hi );
		return SimdDouble2{_mm_add_pd( d_hi, d_lo )};
	  #endif
	}

/*
=================================================
	_LongToDouble2 / _ULongToDouble2
=================================================
*/
# if AE_SIMD_AVX >= 31 // AVX512DQ, AVX512VL

	template <typename IT>
	template <typename T> requires( IsSame<T,slong> )
	SimdDouble2	 SimdTInt128<IT>::_LongToDouble2 () C_NE___
	{
		return SimdDouble2{_mm_cvtepi64_pd( _value )};
	}

	template <typename IT>
	template <typename T> requires( IsSame<T,ulong> )
	SimdDouble2	 SimdTInt128<IT>::_ULongToDouble2 () C_NE___
	{
		return SimdDouble2{_mm_cvtepu64_pd( _value )};
	}

# endif
/*
=================================================
	_IntToDouble4 / _UIntToDouble4
=================================================
*/
# ifdef AE_SIMD_SimdDouble4
	template <typename IT>
	template <typename T> requires( IsSame<T,int> )
	SimdDouble4  SimdTInt128<IT>::_IntToDouble4 () C_NE___
	{
		return SimdDouble4{_mm256_cvtepi32_pd( _value )};
	}

	template <typename IT>
	template <typename T> requires( IsSame<T,uint> )
	SimdDouble4  SimdTInt128<IT>::_UIntToDouble4 () C_NE___
	{
		auto	u_lo = _mm_and_si128( _value, _mm_set1_epi32(0xFFFF) );
		auto	u_hi = _mm_srli_epi32( _value, 16 );
		auto	d_lo = _mm256_cvtepi32_pd( u_lo );
		auto	d_hi = _mm256_cvtepi32_pd( u_hi );
				d_hi = _mm256_mul_pd( _mm256_set1_pd(65536.0), d_hi );
		return SimdDouble4{_mm256_add_pd( d_hi, d_lo )};
	}
# endif
/*
=================================================
	ToShort
=================================================
*/
	template <typename IT>
	template <uint Idx>
	auto  SimdTInt128<IT>::ToShort () C_NE___
	{
		StaticAssert( sizeof(Scalar_t) != sizeof(ushort) );

	  #if AE_SIMD_SSE >= 41
		if constexpr( is8 )
		{
			StaticAssert( Idx < 2 );
			if constexpr( Idx == 0 ){
				if constexpr( isI8 )	return SimdShort8{ _mm_cvtepi8_epi16( _value )};
				else					return SimdUShort8{_mm_cvtepu8_epi16( _value )};
			}else{
				auto	high = _mm_shuffle_epi32( _value, _MM_SHUFFLE( 3, 2, 3, 2 ));
				if constexpr( isI8 )	return SimdShort8{ _mm_cvtepi8_epi16( high )};
				else					return SimdUShort8{_mm_cvtepu8_epi16( high )};
			}
		}else

		if constexpr( isU32 )
		{
			StaticAssert( Idx == 0 );
			Native_t	a = _mm_and_si128( _value, _mm_set1_epi32( 0x0000'FFFF ));
			return SimdUShort8{ _mm_packus_epi32( a, _mm_setzero_si128() )};
		}else
	  #endif

		if constexpr( isU32 )
		{
			StaticAssert( Idx == 0 );
			auto	hi	= _mm_shuffle_epi32( _value,   _MM_SHUFFLE( 3, 2, 3, 2 ));
			auto	a	= _mm_shufflelo_epi16( _value, _MM_SHUFFLE( 2, 0, 2, 0 ));
			auto	b	= _mm_shufflelo_epi16( hi,     _MM_SHUFFLE( 2, 0, 2, 0 ));
			auto	c	= _mm_unpacklo_epi32( a, b );
			return SimdUShort8{_mm_unpacklo_epi64( c, _mm_setzero_si128() )};
		}

		/*if constexpr( isI32 )
		{
			StaticAssert( Idx == 0 );
			return SimdShort8{ _mm_packs_epi32( _value, _mm_setzero_si128() )};
		}*/
	}

/*
=================================================
	ToInt
=================================================
*/
	template <typename IT>
	template <uint Idx>
	auto  SimdTInt128<IT>::ToInt () C_NE___
	{
		StaticAssert( sizeof(Scalar_t) != sizeof(uint) );

	  #if AE_SIMD_SSE >= 41
		if constexpr( is8 )
		{
			StaticAssert( Idx < 4 );
			if constexpr( Idx == 0 ){
				if constexpr( isI8 )	return SimdInt4{ _mm_cvtepi8_epi32( _value )};
				else					return SimdUInt4{_mm_cvtepu8_epi32( _value )};
			}else{
				auto	p = _mm_shuffle_epi32( _value, _MM_SHUFFLE( 0, 0, 0, Idx ));
				if constexpr( isI8 )	return SimdInt4{ _mm_cvtepi8_epi32( p )};
				else					return SimdUInt4{_mm_cvtepu8_epi32( p )};
			}
		}else

		if constexpr( is16 )
		{
			StaticAssert( Idx < 2 );
			if constexpr( Idx == 0 ){
				if constexpr( isI16 )	return SimdInt4{ _mm_cvtepi16_epi32( _value )};
				else					return SimdUInt4{_mm_cvtepu16_epi32( _value )};
			}else{
				auto	high = _mm_shuffle_epi32( _value, _MM_SHUFFLE( 3, 2, 3, 2 ));
				if constexpr( isI16 )	return SimdInt4{ _mm_cvtepi16_epi32( high )};
				else					return SimdUInt4{_mm_cvtepu16_epi32( high )};
			}
		}else
	  #endif

		if constexpr( isU16 )
		{
			StaticAssert( Idx < 2 );
			if constexpr( Idx == 0 ){
				auto	v0123	= Swizzle<0,0,1,1,2,2,3,3>().Ref();
				auto	mask	= _mm_set1_epi32( 0x0000'FFFF );
				return SimdUInt4{ _mm_and_si128( v0123, mask )};
			}else{
				auto	v4567	= Swizzle<4,4,5,5,6,6,7,7>().Ref();
				auto	mask	= _mm_set1_epi32( 0x0000'FFFF );
				return SimdUInt4{ _mm_and_si128( v4567, mask )};
			}
		}
	}

/*
=================================================
	ToLong
=================================================
*/
	template <typename IT>
	template <uint Idx>
	auto  SimdTInt128<IT>::ToLong () C_NE___
	{
		StaticAssert( sizeof(Scalar_t) != sizeof(ulong) );

	  #if AE_SIMD_SSE >= 41
		if constexpr( is8 )
		{
			StaticAssert( Idx < 8 );
			if constexpr( Idx == 0 ){
				if constexpr( isI8 )	return SimdLong2{ _mm_cvtepi8_epi64( _value )};
				else					return SimdULong2{_mm_cvtepu8_epi64( _value )};
			}else
			if constexpr( Idx < 4 ){
				auto	p = _mm_shufflelo_epi16( _value, _MM_SHUFFLE( 0, 0, 0, Idx&3 ));
				if constexpr( isI8 )	return SimdLong2{ _mm_cvtepi8_epi64( p )};
				else					return SimdULong2{_mm_cvtepu8_epi64( p )};
			}else{
				auto	p = _mm_shuffle_epi32( _value, _MM_SHUFFLE( 0, 0, 0, Idx/2 ));
						p = _mm_shufflelo_epi16( p, _MM_SHUFFLE( 0, 0, 0, Idx&1 ));
				if constexpr( isI8 )	return SimdLong2{ _mm_cvtepi8_epi64( p )};
				else					return SimdULong2{_mm_cvtepu8_epi64( p )};
			}
		}

		if constexpr( is16 )
		{
			StaticAssert( Idx < 4 );
			if constexpr( Idx == 0 ){
				if constexpr( isI16 )	return SimdLong2{ _mm_cvtepi16_epi64( _value )};
				else					return SimdULong2{_mm_cvtepu16_epi64( _value )};
			}else{
				auto	p = _mm_shuffle_epi32( _value, _MM_SHUFFLE( 0, 0, 0, Idx ));
				if constexpr( isI16 )	return SimdLong2{ _mm_cvtepi16_epi64( p )};
				else					return SimdULong2{_mm_cvtepu16_epi64( p )};
			}
		}

		if constexpr( is32 )
		{
			StaticAssert( Idx < 2 );
			if constexpr( Idx == 0 ){
				if constexpr( isI32 )	return SimdLong2{ _mm_cvtepi32_epi64( _value )};
				else					return SimdULong2{_mm_cvtepu32_epi64( _value )};
			}else{
				auto	high = _mm_shuffle_epi32( _value, _MM_SHUFFLE( 3, 2, 3, 2 ));
				if constexpr( isI32 )	return SimdLong2{ _mm_cvtepi32_epi64( high )};
				else					return SimdULong2{_mm_cvtepu32_epi64( high )};
			}
		}
	  #endif
	}

/*
=================================================
	ToDouble
=================================================
*/
	template <typename IT>
	template <uint Idx>
	auto  SimdTInt128<IT>::ToDouble () C_NE___
	{
		if constexpr( isI32 )
			return _IntToDouble2<Idx>();

		if constexpr( isU32 )
			return _UIntToDouble2<Idx>();

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
	}

/*
=================================================
	ToDouble4
=================================================
*/
# if AE_SIMD_AVX >= 1
	template <typename IT>
	template <uint Idx>
	auto  SimdTInt128<IT>::ToDouble4 () C_NE___
	{
		if constexpr( isI32 )
			return _IntToDouble4();

		if constexpr( isU32 )
			return _UIntToDouble4();

		if constexpr( is8 or is16 )
			return ToInt< Idx >().ToDouble4();
	}
# endif
/*
=================================================
	ToHalf
=================================================
*/
# ifdef AE_SIMD_SimdHalf8
	template <typename IT>
	template <uint Idx>
	auto  SimdTInt128<IT>::ToHalf () C_NE___
	{
		if constexpr( is32 )
		{
			StaticAssert( Idx == 0 );
			return ToFloat().ToHalf();
		}
		if constexpr( is16 )
		{
			StaticAssert( Idx == 0 );
			auto	low  = ToFloat<0>();
			auto	high = ToFloat<1>();
			return SimdHalf8{ low, high };
		}
		if constexpr( is8 )
		{
			StaticAssert( Idx < 2 );
			auto	low  = ToFloat<Idx*2>();
			auto	high = ToFloat<Idx*2+1>();
			return SimdHalf8{ low, high };
		}
	}
# endif
/*
=================================================
	ToFloat
=================================================
*/
	template <typename IT>
	template <uint Idx>
	auto  SimdTInt128<IT>::ToFloat () C_NE___
	{
		if constexpr( isI32 )
		{
			StaticAssert( Idx == 0 );
			return SimdFloat4{_mm_cvtepi32_ps( _value )};
		}else
		if constexpr( isU32 )
		{
			StaticAssert( Idx == 0 );
		  #if AE_SIMD_AVX >= 31  // AVX512VL
			return SimdFloat4{_mm_cvtepu32_ps( _value )};
		  #else
			auto	u_lo = _mm_and_si128( _value, _mm_set1_epi32(0xFFFF) );
			auto	u_hi = _mm_srli_epi32( _value, 16 );
			auto	f_lo = _mm_cvtepi32_ps( u_lo );
			auto	f_hi = _mm_cvtepi32_ps( u_hi );
					f_hi = _mm_mul_ps( _mm_set1_ps(65536.0f), f_hi );
			return SimdFloat4{_mm_add_ps( f_hi, f_lo )};
		  #endif
		}else
	  #if AE_SIMD_AVX >= 31 // AVX512DQ, AVX512VL
		if constexpr( isI64 )
		{
			StaticAssert( Idx == 0 );
			return _LongToDouble2().ToFloat();
		}else
		if constexpr( isU64 )
		{
			StaticAssert( Idx == 0 );
			return _ULongToDouble2().ToFloat();
		}else
	  #endif
		if constexpr( is8 or is16 )
			return ToInt< Idx >().ToFloat();
	}

/*
=================================================
	ToSigned
=================================================
*/
	template <typename IT>
	template <typename T> requires( IsUnsignedInteger<T> )
	auto  SimdTInt128<IT>::ToSigned () C_NE___
	{
		using S = ToSignedInteger<IT>;
		return BitCast< SimdTInt128<S> >().Max( S{0} );
	}

/*
=================================================
	ToUnsigned
=================================================
*/
	template <typename IT>
	template <typename T> requires( IsSignedInteger<T> )
	auto  SimdTInt128<IT>::ToUnsigned () C_NE___
	{
		using U = ToUnsignedInteger<IT>;
		return Max( IT{0} ).template BitCast< SimdTInt128<U> >();
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
	auto  SimdTInt128<IT>::Convert ()  C_NE___
	{
		StaticAssert( IsAnyScalar< DstScalar >);
		StaticAssert( not IsSame< DstScalar, Scalar_t >);
		StaticAssert( Has_Convert< DstScalar >() );

	  #if AE_SIMD_SSE >= 41
		if constexpr( IsInteger< DstScalar >)
		{
			using Dst = SimdTInt128<DstScalar>;

			if constexpr( IsSame< DstScalar, ulong >)
			{
				if constexpr( isU8 )	return std::array{	ToLong<0>(), ToLong<1>(), ToLong<2>(), ToLong<3>(),
															ToLong<4>(), ToLong<5>(), ToLong<6>(), ToLong<7>() };
				if constexpr( isI8 )	return std::array{	ToLong<0>().template BitCast<Dst>(), ToLong<1>().template BitCast<Dst>(),
															ToLong<2>().template BitCast<Dst>(), ToLong<3>().template BitCast<Dst>(),
															ToLong<4>().template BitCast<Dst>(), ToLong<5>().template BitCast<Dst>(),
															ToLong<6>().template BitCast<Dst>(), ToLong<7>().template BitCast<Dst>() };
				if constexpr( isU16 )	return std::array{	ToLong<0>(), ToLong<1>(), ToLong<2>(), ToLong<3>() };
				if constexpr( isI16 )	return std::array{	ToLong<0>().template BitCast<Dst>(), ToLong<1>().template BitCast<Dst>(),
															ToLong<2>().template BitCast<Dst>(), ToLong<3>().template BitCast<Dst>() };
				if constexpr( isU32 )	return std::array{	ToLong<0>(), ToLong<1>() };
				if constexpr( isI32 )	return std::array{	ToLong<0>().template BitCast<Dst>(), ToLong<1>().template BitCast<Dst>() };
				if constexpr( isI64 )	return BitCast<Dst>();
			}
			if constexpr( IsSame< DstScalar, slong >)
			{
				if constexpr( isI8 )	return std::array{	ToLong<0>(), ToLong<1>(), ToLong<2>(), ToLong<3>(),
															ToLong<4>(), ToLong<5>(), ToLong<6>(), ToLong<7>() };
				if constexpr( isU8 )	return std::array{	ToLong<0>().template BitCast<Dst>(), ToLong<1>().template BitCast<Dst>(),
															ToLong<2>().template BitCast<Dst>(), ToLong<3>().template BitCast<Dst>(),
															ToLong<4>().template BitCast<Dst>(), ToLong<5>().template BitCast<Dst>(),
															ToLong<6>().template BitCast<Dst>(), ToLong<7>().template BitCast<Dst>() };
				if constexpr( isI16 )	return std::array{	ToLong<0>(), ToLong<1>(), ToLong<2>(), ToLong<3>() };
				if constexpr( isU16 )	return std::array{	ToLong<0>().template BitCast<Dst>(), ToLong<1>().template BitCast<Dst>(),
															ToLong<2>().template BitCast<Dst>(), ToLong<3>().template BitCast<Dst>() };
				if constexpr( isI32 )	return std::array{	ToLong<0>(), ToLong<1>() };
				if constexpr( isU32 )	return std::array{	ToLong<0>().template BitCast<Dst>(), ToLong<1>().template BitCast<Dst>() };
				if constexpr( isU64 )	return BitCast<Dst>();
			}

			if constexpr( IsSame< DstScalar, uint >)
			{
				if constexpr( isU8 )	return std::array{	ToInt<0>(), ToInt<1>(), ToInt<2>(), ToInt<3>() };
				if constexpr( isI8 )	return std::array{	ToInt<0>().template BitCast<Dst>(), ToInt<1>().template BitCast<Dst>(),
															ToInt<2>().template BitCast<Dst>(), ToInt<3>().template BitCast<Dst>() };
				if constexpr( isU16 )	return std::array{	ToInt<0>(), ToInt<1>() };
				if constexpr( isI16 )	return std::array{	ToInt<0>().template BitCast<Dst>(), ToInt<1>().template BitCast<Dst>() };
				if constexpr( isI32 )	return BitCast<Dst>();
			}
			if constexpr( IsSame< DstScalar, sint >)
			{
				if constexpr( isI8 )	return std::array{	ToInt<0>(), ToInt<1>(), ToInt<2>(), ToInt<3>() };
				if constexpr( isU8 )	return std::array{	ToInt<0>().template BitCast<Dst>(), ToInt<1>().template BitCast<Dst>(),
															ToInt<2>().template BitCast<Dst>(), ToInt<3>().template BitCast<Dst>() };
				if constexpr( isI16 )	return std::array{	ToInt<0>(), ToInt<1>() };
				if constexpr( isU16 )	return std::array{	ToInt<0>().template BitCast<Dst>(), ToInt<1>().template BitCast<Dst>() };
				if constexpr( isU32 )	return BitCast<Dst>();
			}

			if constexpr( IsSame< DstScalar, ushort >)
			{
				if constexpr( isU8  )	return std::array{	ToShort<0>(), ToShort<1>() };
				if constexpr( isI8  )	return std::array{	ToShort<0>().template BitCast<Dst>(), ToShort<1>().template BitCast<Dst>() };
				if constexpr( isI16 )	return BitCast<Dst>();
				//if constexpr( isU32 or isU64 )	return ToShort();
				//if constexpr( isI32 or isI64 )	return ToShort().template BitCast<Dst>();
			}
			if constexpr( IsSame< DstScalar, sshort >)
			{
				if constexpr( isI8  )	return std::array{	ToShort<0>(), ToShort<1>() };
				if constexpr( isU8  )	return std::array{	ToShort<0>().template BitCast<Dst>(), ToShort<1>().template BitCast<Dst>() };
				if constexpr( isU16 )	return BitCast<Dst>();
				//if constexpr( isI32 or isI64 )	return ToShort();
				//if constexpr( isU32 or isU64 )	return ToShort().template BitCast<Dst>();
			}
		}
	  #endif

	  #if AE_SIMD_AVX >= 2
		// TODO
	  #endif

	  #ifdef AE_SIMD_SimdHalf8
		if constexpr( IsSame< DstScalar, half >)
		{
			if constexpr( is8 )				return std::array{ ToHalf<0>(), ToHalf<1>() };
			if constexpr( is16 or is32 )	return ToHalf();
		}
	  #endif

		if constexpr( IsSame< DstScalar, float >)
		{
			if constexpr( is32 )	return ToFloat();
			if constexpr( is16 )	return std::array{ ToFloat<0>(), ToFloat<1>() };
			if constexpr( is8 )		return std::array{ ToFloat<0>(), ToFloat<1>(), ToFloat<2>(), ToFloat<3>() };
		}

		if constexpr( IsSame< DstScalar, double > )
		{
			if constexpr( is32 ) {
			  #if AE_SIMD_AVX >= 1
				return ToDouble4();
			  #else
				return std::array{ ToDouble<0>(), ToDouble<1>() };
			  #endif
			}
			if constexpr( is16 ) {
			  #if AE_SIMD_AVX >= 1
				return std::array{	ToDouble4<0>(), ToDouble4<1>() };
			  #else
				return std::array{	ToDouble<0>(), ToDouble<1>(), ToDouble<2>(), ToDouble<3>() };
			  #endif
			}
			if constexpr( is8 ) {
			  #if AE_SIMD_AVX >= 1
				return std::array{	ToDouble4<0>(), ToDouble4<1>(), ToDouble4<2>(), ToDouble4<3>() };
			  #else
				return std::array{	ToDouble<0>(), ToDouble<1>(), ToDouble<2>(), ToDouble<3>(),
									ToDouble<4>(), ToDouble<5>(), ToDouble<6>(), ToDouble<7>()};
			  #endif
			}
		}

	  #if AE_SIMD_AVX >= 31  // AVX512DQ, AVX512VL
		if constexpr( IsSame< DstScalar, double > and is64 )
		{
			if constexpr( isI64 )	return _LongToDouble2();
			else					return _ULongToDouble2();
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
	__Ce__ bool  SimdTInt128<IT>::Has_Convert ()
	{
		if constexpr( IsSame< DstScalar, Scalar_t >)
			return false;
		else
	  #if AE_SIMD_SSE >= 41
		if constexpr( IsInteger< DstScalar >)
			return	(sizeof(DstScalar) == sizeof(ulong)		and sizeof(IT) <= sizeof(ulong))	or
					(sizeof(DstScalar) == sizeof(uint)		and sizeof(IT) <= sizeof(uint))		or
					(sizeof(DstScalar) == sizeof(ushort)	and sizeof(IT) <= sizeof(ushort));
		else
	  #endif
	  #ifdef AE_SIMD_SimdHalf8
		if constexpr( IsSame< DstScalar, half >)
			return Has_Convert<float>();
		else
	  #endif
		if constexpr( IsSame< DstScalar, float >)
			return is32 or Has_Convert<int>();
		else
		if constexpr( IsSame< DstScalar, double > and is32 )
			return true;
		else
	  #if AE_SIMD_AVX >= 31  // AVX512DQ, AVX512VL
		if constexpr( IsSame< DstScalar, double > and is64 )
			return true;
		else
	  #endif
			return false;
	}

/*
=================================================
	Select
=================================================
*/
	template <typename IT>
	SimdTInt128<IT>  SimdTInt128<IT>::_Select (const typename SimdTInt128<IT>::Bool_t &condition, const SimdTInt128<IT> &ifTrue, const SimdTInt128<IT> &ifFalse) __NE___
	{
	  #if AE_SIMD_SSE >= 41
		return SimdTInt128<IT>{ _mm_blendv_epi8( ifFalse._value, ifTrue._value, condition.Ref() )};
	  #else
		auto	a = _mm_andnot_si128( condition.Ref(), ifFalse._value );
		auto	b = _mm_and_si128( condition.Ref(), ifTrue._value );
		return SimdTInt128<IT>{ _mm_or_si128( a, b )};
	  #endif
	}

#endif // AE_SIMD_SimdTInt128
//-----------------------------------------------------------------------------



#ifdef AE_SIMD_Int128b
/*
=================================================
	ToArray
=================================================
*/
	template <typename T>
	auto  Int128b::ToArray ()  C_NE___
	{
		StaticAssert( IsInteger<T> );
		StaticArray< T, sizeof(Native_t) / sizeof(T) >	arr;
		_mm_storeu_si128( OUT reinterpret_cast<Native_t *>( arr.data() ), _value );
		return arr;
	}

#endif // AE_SIMD_Int128b
} // AE::Base
//-----------------------------------------------------------------------------


#include "base/SIMD/FloatConversion.h"

namespace AE::Base
{
#ifdef AE_SIMD_SimdHalf8
/*
=================================================
	cvtps_ph / cvtph_ps / cvtph_ps256
=================================================
*/
# if AE_SIMD_F16C
namespace{
	Nd__IF __m128i  cvtps_ph	(__m128  src)	__NE___	{ return _mm_cvtps_ph( src, _MM_FROUND_NINT ); }
	Nd__IF __m128   cvtph_ps	(__m128i src)	__NE___	{ return _mm_cvtph_ps( src ); }
#  ifdef AE_SIMD_SimdFloat8
	Nd__IF __m128i  cvtps256_ph	(__m256  src)	__NE___	{ return _mm256_cvtps_ph( src, _MM_FROUND_NINT ); }
	Nd__IF __m256   cvtph_ps256	(__m128i src)	__NE___	{ return _mm256_cvtph_ps( src ); }
#  endif
}
# else
namespace{
	Nd__IF __m128i  cvtps_ph (__m128  srcF)		__NE___	{ return SimdFloatConversion::FloatToHalf( SimdFloat4{srcF} ).Ref(); }
	Nd__IF __m128   cvtph_ps (__m128i srcH)		__NE___ { return SimdFloatConversion::HalfToFloat( SimdUShort8{srcH} ).Ref(); }
#  ifdef AE_SIMD_SimdFloat8
	Nd__IF __m128i  cvtps256_ph	(__m256  srcF)	__NE___	{ return SimdFloatConversion::FloatToHalf( SimdFloat8{srcF} ).Ref(); }
	Nd__IF __m256   cvtph_ps256	(__m128i srcH)	__NE___	{ return SimdFloatConversion::HalfToFloat8( SimdUShort8{srcH} ).Ref(); }
#  endif
}
# endif // AE_SIMD_F16C

/*
=================================================
	constructor
=================================================
*/
	inline SimdHalf8::SimdHalf8 (Scalar_t v0, Scalar_t v1, Scalar_t v2, Scalar_t v3,
								 Scalar_t v4, Scalar_t v5, Scalar_t v6, Scalar_t v7) __NE___ :
		_value{ _mm_set_epi16(	Base::BitCast<short>(v7), Base::BitCast<short>(v6), Base::BitCast<short>(v5), Base::BitCast<short>(v4),
								Base::BitCast<short>(v3), Base::BitCast<short>(v2), Base::BitCast<short>(v1), Base::BitCast<short>(v0) )}
	{}

	inline SimdHalf8::SimdHalf8 (const SimdFloat4 &v0123) __NE___ :
		_value{ cvtps_ph( v0123.Ref() )}
	{}

	inline SimdHalf8::SimdHalf8 (const SimdFloat4 &v0123, const SimdFloat4 &v4567) __NE___
	{
		auto	lo = cvtps_ph( v0123.Ref() );
		auto	hi = cvtps_ph( v4567.Ref() );
		_value = _mm_unpacklo_epi64( lo, hi );
	}

	inline SimdHalf8::SimdHalf8 (float v0, float v1, float v2, float v3,
								 float v4, float v5, float v6, float v7) __NE___
	{
		auto	h0246 = cvtps_ph( _mm_set_ps( v6, v4, v2, v0 ));
		auto	h1357 = cvtps_ph( _mm_set_ps( v7, v5, v3, v1 ));
		_value = _mm_unpacklo_epi16( h0246, h1357 );
	}

/*
=================================================
	get
=================================================
*/
	template <uint I>
	half  SimdHalf8::get ()  C_NE___
	{
		StaticAssert( I < count );
		return Base::BitCast<half>( ushort(_mm_extract_epi16( _value, I )) );
	}

/*
=================================================
	set
=================================================
*/
	template <uint I>
	SimdHalf8  SimdHalf8::set (Scalar_t newValue)  C_NE___
	{
		StaticAssert( I < count );
		return Self{_mm_insert_epi16( _value, Base::BitCast<ushort>(newValue), I )};
	}

/*
=================================================
	Select
=================================================
*/
	inline SimdHalf8  Select (const typename SimdHalf8::Bool8 &condition, const SimdHalf8 &ifTrue, const SimdHalf8 &ifFalse) __NE___
	{
	  #if AE_SIMD_SSE >= 41
		return SimdHalf8{ _mm_blendv_epi8( ifFalse._value, ifTrue._value, condition.Ref() )};
	  #else
		auto	a = _mm_andnot_si128( condition.Ref(), ifFalse._value );
		auto	b = _mm_and_si128( condition.Ref(), ifTrue._value );
		return SimdHalf8{ _mm_or_si128( a, b )};
	  #endif
	}

/*
=================================================
	BitCast
=================================================
*/
	template <typename DstType>
	DstType  SimdHalf8::BitCast () C_NE___
	{
		StaticAssert( sizeof(Self) == sizeof(DstType) );
		StaticAssert( Has_BitCast<DstType>() );

		if constexpr( IsSame< DstType, SimdInt_t > or IsSame< DstType, SimdUInt_t >)
			return DstType{ _value };
	}

/*
=================================================
	Has_BitCast
=================================================
*/
	template <typename DstType>
	__Ce__ bool  SimdHalf8::Has_BitCast ()
	{
		if constexpr( IsSame< DstType, SimdInt_t > or IsSame< DstType, SimdUInt_t >)
			return true;
		else
			return false;
	}

/*
=================================================
	ToFloat / ToFloat8
=================================================
*/
	template <uint I>
	auto  SimdHalf8::ToFloat () C_NE___
	{
		StaticAssert( I < 2 );
		if constexpr( I == 0 ){
			return SimdFloat4{ cvtph_ps( _value )};
		}else{
			auto	hi = _mm_shuffle_epi32( _value, _MM_SHUFFLE( 3, 2, 3, 2 ));
			return SimdFloat4{ cvtph_ps( hi )};
		}
	}

# ifdef AE_SIMD_SimdFloat8
	inline SimdFloat8  SimdHalf8::ToFloat8 () C_NE___
	{
		return SimdFloat8{ cvtph_ps256( _value )};
	}
# endif
/*
=================================================
	Swizzle
=================================================
*/
	template <uint V0, uint V1, uint V2, uint V3, uint V4, uint V5, uint V6, uint V7>
	SimdHalf8  SimdHalf8::Swizzle () C_NE___
	{
		StaticAssert( Has_Swizzle() );
		return BitCast<SimdShort8>()
				.template Swizzle<V0,V1,V2,V3,V4,V5,V6,V7>()
				.template BitCast<SimdHalf8>();
	}

/*
=================================================
	Convert
----
	same as C++ style conversion 'Dst(src)'
=================================================
*/
	template <typename DstScalar>
	auto  SimdHalf8::Convert () C_NE___
	{
		StaticAssert( IsAnyScalar< DstScalar >);
		StaticAssert( not IsSame< DstScalar, Scalar_t >);
		StaticAssert( Has_Convert< DstScalar >() );

		if constexpr( IsSame< DstScalar, float >)
		{
		  #if AE_SIMD_AVX >= 1
			return ToFloat8();
		  #else
			return std::array{ ToFloat<0>(), ToFloat<1>() };
		  #endif
		}
	}

/*
=================================================
	Has_Convert
=================================================
*/
	template <typename DstScalar>
	__Ce__ bool  SimdHalf8::Has_Convert ()
	{
		if constexpr( IsSame< DstScalar, Scalar_t >)
			return false;
		else
		if constexpr( IsSame< DstScalar, float >)
			return true;
		else
			return false;
	}

#endif // AE_SIMD_SimdHalf8
//-----------------------------------------------------------------------------

} // AE::Base
