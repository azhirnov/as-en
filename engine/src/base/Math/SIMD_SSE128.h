// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	128 bit SIMD
*/

#pragma once

#include "base/Math/GLM.h"

namespace AE::Math
{
# if AE_SIMD_SSE > 0


	//
	// 128 bit float (SSE 2.x)
	//
#	define AE_SIMD_SimdFloat4
	struct SimdFloat4
	{
	// types
	public:
		using Value_t	= float;
		using Self		= SimdFloat4;

		struct Bool4
		{
		private:
			__m128i		_value;

		public:
			explicit Bool4 (__m128 val)					__NE___	: _value{_mm_castps_si128(val)} {}
			explicit Bool4 (__m128i val)				__NE___	: _value{val} {}
			Bool4 (const SimdInt4 &v)					__NE___;
			Bool4 (const SimdUInt4 &v)					__NE___;

			ND_ Bool4  operator | (const Bool4 &rhs)	C_NE___	{ return Bool4{ _mm_or_si128( _value, rhs._value )}; }
			ND_ Bool4  operator & (const Bool4 &rhs)	C_NE___	{ return Bool4{ _mm_and_si128( _value, rhs._value )}; }
			ND_ Bool4  operator ^ (const Bool4 &rhs)	C_NE___	{ return Bool4{ _mm_xor_si128( _value, rhs._value )}; }
			ND_ Bool4  operator ~ ()					C_NE___	{ return Bool4{ _mm_andnot_si128( _value, _mm_set1_epi32(-1) )}; }
			ND_ bool   operator [] (usize i)			C_NE___	{ ASSERT( i < 4 );  return _value.m128i_u32[i] == UMax; }

			ND_ bool  All ()							C_NE___	{ return (_value.m128i_u32[0] & _value.m128i_u32[1] & _value.m128i_u32[2] & _value.m128i_u32[3]) == UMax; }
			ND_ bool  Any ()							C_NE___	{ return (_value.m128i_u32[0] | _value.m128i_u32[1] | _value.m128i_u32[2] | _value.m128i_u32[3]) == UMax; }
			ND_ bool  None ()							C_NE___	{ return (_value.m128i_u32[0] | _value.m128i_u32[1] | _value.m128i_u32[2] | _value.m128i_u32[3]) == 0; }
		};


	// variables
	private:
		__m128	_value;		// float[4]


	// methods
	public:
		SimdFloat4 ()										__NE___	: _value{ _mm_setzero_ps() } {}
		explicit SimdFloat4 (float v)						__NE___	: _value{ _mm_set1_ps( v )} {}
		explicit SimdFloat4 (const float* ptr)				__NE___	: _value{ _mm_loadu_ps( ptr )} { ASSERT( ptr != null ); }
		explicit SimdFloat4 (const __m128 &v)				__NE___	: _value{ v } {}
		SimdFloat4 (float x, float y, float z, float w)		__NE___	: _value{ _mm_set_ps( x, y, z, w )} {}

		ND_ Self  operator +  (const Self &rhs)				C_NE___	{ return Add( rhs ); }
		ND_ Self  operator -  (const Self &rhs)				C_NE___	{ return Sub( rhs ); }
		ND_ Self  operator *  (const Self &rhs)				C_NE___	{ return Mul( rhs ); }
		ND_ Self  operator /  (const Self &rhs)				C_NE___	{ return Div( rhs ); }
		
		ND_ Self  operator &  (const Self &rhs)				C_NE___	{ return And( rhs ); }
		ND_ Self  operator |  (const Self &rhs)				C_NE___	{ return Or( rhs ); }
		ND_ Self  operator ^  (const Self &rhs)				C_NE___	{ return Xor( rhs ); }

		ND_ Bool4  operator == (const Self &rhs)			C_NE___	{ return Equal( rhs ); }
		ND_ Bool4  operator != (const Self &rhs)			C_NE___	{ return NotEqual( rhs ); }
		ND_ Bool4  operator >  (const Self &rhs)			C_NE___	{ return Greater( rhs ); }
		ND_ Bool4  operator <  (const Self &rhs)			C_NE___	{ return Less( rhs ); }
		ND_ Bool4  operator >= (const Self &rhs)			C_NE___	{ return GEqual( rhs ); }
		ND_ Bool4  operator <= (const Self &rhs)			C_NE___	{ return LEqual( rhs ); }

		ND_ Value_t  operator [] (usize i)					C_NE___	{ ASSERT( i < 4 );  return _value.m128_f32[i]; }
		ND_ Value_t  GetX ()								C_NE___	{ return _mm_cvtss_f32( _value ); }

		ND_ __m128 const&  Get ()							C_NE___	{ return _value; }

		ND_ Self  Add (const Self &rhs)						C_NE___	{ return Self{ _mm_add_ps( _value, rhs._value )}; }
		ND_ Self  Sub (const Self &rhs)						C_NE___	{ return Self{ _mm_sub_ps( _value, rhs._value )}; }
		ND_ Self  Mul (const Self &rhs)						C_NE___	{ return Self{ _mm_mul_ps( _value, rhs._value )}; }
		ND_ Self  Div (const Self &rhs)						C_NE___	{ return Self{ _mm_div_ps( _value, rhs._value )}; }
		ND_ Self  Min (const Self &rhs)						C_NE___	{ return Self{ _mm_min_ps( _value, rhs._value )}; }
		ND_ Self  Max (const Self &rhs)						C_NE___	{ return Self{ _mm_max_ps( _value, rhs._value )}; }

		ND_ Self  And (const Self &rhs)						C_NE___	{ return Self{ _mm_and_ps( _value, rhs._value )}; }
		ND_ Self  Or  (const Self &rhs)						C_NE___	{ return Self{ _mm_or_ps( _value, rhs._value )}; }
		ND_ Self  Xor (const Self &rhs)						C_NE___	{ return Self{ _mm_xor_ps( _value, rhs._value )}; }
		ND_ Self  AndNot (const Self &rhs)					C_NE___	{ return Self{ _mm_andnot_ps( _value, rhs._value )}; }		// !a & b
		
	  #if AE_SIMD_SSE >= 30
		ND_ Self  HAdd (const Self &rhs)					C_NE___	{ return Self{ _mm_hadd_ps( _value, rhs._value )}; }		// { a0 + a1, a2 + a3, b0 + b1, b2 + b3 }
		ND_ Self  HSub (const Self &rhs)					C_NE___	{ return Self{ _mm_hsub_ps( _value, rhs._value )}; }		// { a0 - a1, a2 - a3, b0 - b1, b2 - b3 }

		ND_ Self  AddSub (const Self &rhs)					C_NE___	{ return Self{ _mm_addsub_ps( _value, rhs._value )}; }		// { a0 - b0, a1 + b1, a2 - b2, a3 + b3 }
	  #endif

		ND_ Self  FMAdd (const Self &b, const Self &c)		C_NE___ { return Self{ _mm_fmadd_ps(    _value, b._value, c._value )}; }	// a * b + c
		ND_ Self  FMSub (const Self &b, const Self &c)		C_NE___ { return Self{ _mm_fmsub_ps(    _value, b._value, c._value )}; }	// a * b - c
		ND_ Self  FMAddSub (const Self &b, const Self &c)	C_NE___ { return Self{ _mm_fmaddsub_ps( _value, b._value, c._value )}; }	// { a0 * b0 - c0, a1 * b1 + c1, a2 * b2 - c2, a3 * b3 + c3 }
		ND_ Self  FMSubAdd (const Self &b, const Self &c)	C_NE___ { return Self{ _mm_fmaddsub_ps( _value, b._value, c._value )}; }	// { a0 * b0 + c0, a1 * b1 - c1, a2 * b2 + c2, a3 * b3 - c3 }
		ND_ Self  FNMAdd (const Self &b, const Self &c)		C_NE___ { return Self{ _mm_fnmadd_ps(   _value, b._value, c._value )}; }	// -a * b + c
		ND_ Self  FNMSub (const Self &b, const Self &c)		C_NE___ { return Self{ _mm_fnmsub_ps(   _value, b._value, c._value )}; }	// -a * b - c

		ND_ Self  Sqrt ()									C_NE___	{ return Self{ _mm_sqrt_ps( _value )}; }
		ND_ Self  Reciporal ()								C_NE___	{ return Self{ _mm_rcp_ps( _value )}; }							// 1 / x
		ND_ Self  RSqrt ()									C_NE___	{ return Self{ _mm_rsqrt_ps( _value )}; }						// 1 / sqrt(x)
		ND_ Self  FastSqrt ()								C_NE___	{ return Self{ _mm_mul_ps( _value, _mm_rsqrt_ps( _value ))}; }	// x / sqrt(x)
		ND_ Self  Scale (const Self &rhs)					C_NE___	{ return Self{ _mm_scalef_ps( _value, rhs._value )}; }			// a * 2^b

		ND_ Self  Abs ()									C_NE___
		{
			__m128	signmask = _mm_set1_ps( -0.0f );
			return Self{ _mm_andnot_ps( signmask, _value )};
		}

		ND_ Self  AddScalar (const Self &rhs)				C_NE___	{ return Self{ _mm_add_ss( _value, rhs._value )}; }			// { a0 + b0, a1, a2, a3 }
		ND_ Self  SubScalar (const Self &rhs)				C_NE___	{ return Self{ _mm_sub_ss( _value, rhs._value )}; }			// { a0 - b0, a1, a2, a3 }
		ND_ Self  MulScalar (const Self &rhs)				C_NE___	{ return Self{ _mm_mul_ss( _value, rhs._value )}; }			// { a0 * b0, a1, a2, a3 }
		ND_ Self  DivScalar (const Self &rhs)				C_NE___	{ return Self{ _mm_div_ss( _value, rhs._value )}; }			// { a0 / b0, a1, a2, a3 }
		ND_ Self  SqrtScalar ()								C_NE___	{ return Self{ _mm_sqrt_ss( _value )}; }					// { sqrt(a0), a1, a2, a3 }
		ND_ Self  RSqrtScalar ()							C_NE___	{ return Self{ _mm_rsqrt_ss( _value )}; }					// { 1 / sqrt(a0), a1, a2, a3 }
		ND_ Self  ReciporalScalar ()						C_NE___	{ return Self{ _mm_rcp_ps( _value )}; }						// { 1 / a0, a1, a2, a3 }

		ND_ Bool4  Equal    (const Self &rhs)				C_NE___	{ return Bool4{ _mm_cmpeq_ps( _value, rhs._value )}; }
		ND_ Bool4  NotEqual (const Self &rhs)				C_NE___	{ return Bool4{ _mm_cmpneq_ps( _value, rhs._value )}; }
		ND_ Bool4  Greater  (const Self &rhs)				C_NE___	{ return Bool4{ _mm_cmpgt_ps( _value, rhs._value )}; }
		ND_ Bool4  Less     (const Self &rhs)				C_NE___	{ return Bool4{ _mm_cmplt_ps( _value, rhs._value )}; }
		ND_ Bool4  GEqual   (const Self &rhs)				C_NE___	{ return Bool4{ _mm_cmpge_ps( _value, rhs._value )}; }
		ND_ Bool4  LEqual   (const Self &rhs)				C_NE___	{ return Bool4{ _mm_cmple_ps( _value, rhs._value )}; }

	  #if AE_SIMD_AVX >= 1	// AVX 1
		// ordered - comparison with NaN returns false
		ND_ Bool4  EqualO    (const Self &rhs)				C_NE___	{ return Bool4{ _mm_cmp_ps( _value, rhs._value, _CMP_EQ_OQ  )}; }
		ND_ Bool4  NotEqualO (const Self &rhs)				C_NE___	{ return Bool4{ _mm_cmp_ps( _value, rhs._value, _CMP_NEQ_OQ )}; }
		ND_ Bool4  GreaterO  (const Self &rhs)				C_NE___	{ return Bool4{ _mm_cmp_ps( _value, rhs._value, _CMP_GT_OQ  )}; }
		ND_ Bool4  LessO     (const Self &rhs)				C_NE___	{ return Bool4{ _mm_cmp_ps( _value, rhs._value, _CMP_LT_OQ  )}; }
		ND_ Bool4  GEqualO   (const Self &rhs)				C_NE___	{ return Bool4{ _mm_cmp_ps( _value, rhs._value, _CMP_GE_OQ  )}; }
		ND_ Bool4  LEqualO   (const Self &rhs)				C_NE___	{ return Bool4{ _mm_cmp_ps( _value, rhs._value, _CMP_LE_OQ  )}; }
		
		// unordered - comparison with NaN returns true
		ND_ Bool4  EqualU    (const Self &rhs)				C_NE___	{ return Bool4{ _mm_cmp_ps( _value, rhs._value, _CMP_EQ_UQ  )}; }
		ND_ Bool4  NotEqualU (const Self &rhs)				C_NE___	{ return Bool4{ _mm_cmp_ps( _value, rhs._value, _CMP_NEQ_UQ )}; }
		ND_ Bool4  GreaterU  (const Self &rhs)				C_NE___	{ return Bool4{ _mm_cmp_ps( _value, rhs._value, _CMP_NLE_UQ )}; }
		ND_ Bool4  LessU     (const Self &rhs)				C_NE___	{ return Bool4{ _mm_cmp_ps( _value, rhs._value, _CMP_NGE_UQ )}; }
		ND_ Bool4  GEqualU   (const Self &rhs)				C_NE___	{ return Bool4{ _mm_cmp_ps( _value, rhs._value, _CMP_NLT_UQ )}; }
		ND_ Bool4  LEqualU   (const Self &rhs)				C_NE___	{ return Bool4{ _mm_cmp_ps( _value, rhs._value, _CMP_NGT_UQ )}; }
	  #endif

		// TODO: _mm_cmp_ps AVX
		
	  #if AE_SIMD_SSE >= 41
		ND_ Self  Floor ()									C_NE___	{ return Self{ _mm_floor_ps( _value )}; }
		ND_ Self  Ceil ()									C_NE___	{ return Self{ _mm_ceil_ps( _value )}; }

		template <int Mode> ND_ Self  Round ()				C_NE___	{ return Self{ _mm_round_ps( _value, Mode )}; }

		ND_ Self  Dot (const Self &rhs)						C_NE___	{ return Self{ _mm_dp_ps( _value, rhs._value, 0xFF )}; }
	  #endif

			void  ToArray (OUT Value_t* dst)				C_NE___	{ _mm_storeu_ps( OUT dst, _value ); }

		template <typename IT>
		ND_ EnableIf< IsInteger<IT>, SimdTInt128<IT>>		Cast ()	C_NE___;

		template <typename DT>
		ND_ EnableIf< IsSameTypes<DT, double>, SimdDouble2>	Cast ()	C_NE___;
	};
	


	//
	// 128 bit double (SSE 2.x)
	//
#	define AE_SIMD_SimdDouble2
	struct SimdDouble2
	{
	// types
	public:
		using Value_t	= double;
		using Self		= SimdDouble2;
		
		struct Bool2
		{
		private:
			__m128i		_value;

		public:
			explicit Bool2 (__m128d val)				__NE___	: _value{_mm_castpd_si128(val)} {}
			explicit Bool2 (__m128i val)				__NE___	: _value{val} {}
			Bool2 (const SimdLong2 &v)					__NE___;
			Bool2 (const SimdULong2 &v)					__NE___;

			ND_ Bool2  operator | (const Bool2 &rhs)	C_NE___	{ return Bool2{ _mm_or_si128( _value, rhs._value )}; }
			ND_ Bool2  operator & (const Bool2 &rhs)	C_NE___	{ return Bool2{ _mm_and_si128( _value, rhs._value )}; }
			ND_ Bool2  operator ^ (const Bool2 &rhs)	C_NE___	{ return Bool2{ _mm_xor_si128( _value, rhs._value )}; }
			ND_ Bool2  operator ~ ()					C_NE___	{ return Bool2{ _mm_andnot_si128( _value, _mm_set1_epi64x(-1) )}; }
			ND_ bool   operator [] (usize i)			C_NE___	{ ASSERT( i < 2 );  return _value.m128i_u64[i] == UMax; }

			ND_ bool  All ()							C_NE___	{ return (_value.m128i_u64[0] & _value.m128i_u64[1]) == UMax; }
			ND_ bool  Any ()							C_NE___	{ return (_value.m128i_u64[0] | _value.m128i_u64[1]) == UMax; }
			ND_ bool  None ()							C_NE___	{ return (_value.m128i_u64[0] | _value.m128i_u64[1]) == 0; }
		};


	// variables
	private:
		__m128d		_value;		// double[2]


	// methods
	public:
		SimdDouble2 ()								__NE___	: _value{ _mm_setzero_pd() } {}
		explicit SimdDouble2 (double v)				__NE___	: _value{ _mm_set1_pd( v )} {}
		explicit SimdDouble2 (const double *v)		__NE___	: _value{ _mm_loadu_pd( v )} {}
		explicit SimdDouble2 (const __m128d &v)		__NE___	: _value{ v } {}
		SimdDouble2 (double x, double y)			__NE___	: _value{_mm_set_pd( x, y )} {}

		ND_ Self  operator +  (const Self &rhs)		C_NE___	{ return Add( rhs ); }
		ND_ Self  operator -  (const Self &rhs)		C_NE___	{ return Sub( rhs ); }
		ND_ Self  operator *  (const Self &rhs)		C_NE___	{ return Mul( rhs ); }
		ND_ Self  operator /  (const Self &rhs)		C_NE___	{ return Div( rhs ); }
		
		ND_ Self  operator &  (const Self &rhs)		C_NE___	{ return And( rhs ); }
		ND_ Self  operator |  (const Self &rhs)		C_NE___	{ return Or( rhs ); }
		ND_ Self  operator ^  (const Self &rhs)		C_NE___	{ return Xor( rhs ); }

		ND_ Bool2  operator == (const Self &rhs)	C_NE___	{ return Equal( rhs ); }
		ND_ Bool2  operator != (const Self &rhs)	C_NE___	{ return NotEqual( rhs ); }
		ND_ Bool2  operator >  (const Self &rhs)	C_NE___	{ return Greater( rhs ); }
		ND_ Bool2  operator <  (const Self &rhs)	C_NE___	{ return Less( rhs ); }
		ND_ Bool2  operator >= (const Self &rhs)	C_NE___	{ return GEqual( rhs ); }
		ND_ Bool2  operator <= (const Self &rhs)	C_NE___	{ return LEqual( rhs ); }
		
		ND_ Value_t  operator [] (usize i)			C_NE___	{ ASSERT( i < 2 );  return _value.m128d_f64[i]; }
		
		ND_ __m128d const&  Get ()					C_NE___	{ return _value; }


		ND_ Self  Add (const Self &rhs)				C_NE___	{ return Self{ _mm_add_pd( _value, rhs._value )}; }
		ND_ Self  Sub (const Self &rhs)				C_NE___	{ return Self{ _mm_sub_pd( _value, rhs._value )}; }
		ND_ Self  Mul (const Self &rhs)				C_NE___	{ return Self{ _mm_mul_pd( _value, rhs._value )}; }
		ND_ Self  Div (const Self &rhs)				C_NE___	{ return Self{ _mm_div_pd( _value, rhs._value )}; }
		ND_ Self  Min (const Self &rhs)				C_NE___	{ return Self{ _mm_min_pd( _value, rhs._value )}; }
		ND_ Self  Max (const Self &rhs)				C_NE___	{ return Self{ _mm_max_pd( _value, rhs._value )}; }
		
		ND_ Self  And (const Self &rhs)				C_NE___	{ return Self{ _mm_and_pd( _value, rhs._value )}; }
		ND_ Self  Or  (const Self &rhs)				C_NE___	{ return Self{ _mm_or_pd( _value, rhs._value )}; }
		ND_ Self  Xor (const Self &rhs)				C_NE___	{ return Self{ _mm_xor_pd( _value, rhs._value )}; }
		ND_ Self  AndNot (const Self &rhs)			C_NE___	{ return Self{ _mm_andnot_pd( _value, rhs._value )}; } // !a & b
		
	  #if AE_SIMD_SSE >= 30
		ND_ Self  HAdd (const Self &rhs)			C_NE___	{ return Self{ _mm_hadd_pd( _value, rhs._value )}; }	// { a0 + a1,  b0 + b1 }
		ND_ Self  HSub (const Self &rhs)			C_NE___	{ return Self{ _mm_hsub_pd( _value, rhs._value )}; }	// { a0 - a1,  b0 - b1 }
		
		ND_ Self  AddSub (const Self &rhs)			C_NE___	{ return Self{ _mm_addsub_pd( _value, rhs._value )}; }	// { a0 - b0, a1 + b1 }
	  #endif

		ND_ Bool2  Equal    (const Self &rhs)		C_NE___	{ return Bool2{ _mm_cmpeq_pd( _value, rhs._value )}; }
		ND_ Bool2  NotEqual (const Self &rhs)		C_NE___	{ return Bool2{ _mm_cmpneq_pd( _value, rhs._value )}; }
		ND_ Bool2  Greater  (const Self &rhs)		C_NE___	{ return Bool2{ _mm_cmpgt_pd( _value, rhs._value )}; }
		ND_ Bool2  Less     (const Self &rhs)		C_NE___	{ return Bool2{ _mm_cmplt_pd( _value, rhs._value )}; }
		ND_ Bool2  GEqual   (const Self &rhs)		C_NE___	{ return Bool2{ _mm_cmpge_pd( _value, rhs._value )}; }
		ND_ Bool2  LEqual   (const Self &rhs)		C_NE___	{ return Bool2{ _mm_cmple_pd( _value, rhs._value )}; }
		
	  #if AE_SIMD_AVX >= 1	// AVX 1
		// ordered - comparison with NaN returns false
		ND_ Bool2  EqualO    (const Self &rhs)		C_NE___	{ return Bool2{ _mm_cmp_pd( _value, rhs._value, _CMP_EQ_OQ  )}; }
		ND_ Bool2  NotEqualO (const Self &rhs)		C_NE___	{ return Bool2{ _mm_cmp_pd( _value, rhs._value, _CMP_NEQ_OQ )}; }
		ND_ Bool2  GreaterO  (const Self &rhs)		C_NE___	{ return Bool2{ _mm_cmp_pd( _value, rhs._value, _CMP_GT_OQ  )}; }
		ND_ Bool2  LessO     (const Self &rhs)		C_NE___	{ return Bool2{ _mm_cmp_pd( _value, rhs._value, _CMP_LT_OQ  )}; }
		ND_ Bool2  GEqualO   (const Self &rhs)		C_NE___	{ return Bool2{ _mm_cmp_pd( _value, rhs._value, _CMP_GE_OQ  )}; }
		ND_ Bool2  LEqualO   (const Self &rhs)		C_NE___	{ return Bool2{ _mm_cmp_pd( _value, rhs._value, _CMP_LE_OQ  )}; }
		
		// unordered - comparison with NaN returns true
		ND_ Bool2  EqualU    (const Self &rhs)		C_NE___	{ return Bool2{ _mm_cmp_pd( _value, rhs._value, _CMP_EQ_UQ  )}; }
		ND_ Bool2  NotEqualU (const Self &rhs)		C_NE___	{ return Bool2{ _mm_cmp_pd( _value, rhs._value, _CMP_NEQ_UQ )}; }
		ND_ Bool2  GreaterU  (const Self &rhs)		C_NE___	{ return Bool2{ _mm_cmp_pd( _value, rhs._value, _CMP_NLE_UQ )}; }
		ND_ Bool2  LessU     (const Self &rhs)		C_NE___	{ return Bool2{ _mm_cmp_pd( _value, rhs._value, _CMP_NGE_UQ )}; }
		ND_ Bool2  GEqualU   (const Self &rhs)		C_NE___	{ return Bool2{ _mm_cmp_pd( _value, rhs._value, _CMP_NLT_UQ )}; }
		ND_ Bool2  LEqualU   (const Self &rhs)		C_NE___	{ return Bool2{ _mm_cmp_pd( _value, rhs._value, _CMP_NGT_UQ )}; }
	  #endif

	  #if AE_SIMD_SSE >= 41
		ND_ Self  Dot (const Self &rhs)				C_NE___
		{
			return Self{ _mm_dp_pd( _value, rhs._value, 0xFF )};
			//__m128d	ab = _mm_mul_pd( _value, rhs._value );
			//return Self{ _mm_add_pd( ab, _mm_shuffle_pd( ab, ab, _MM_SHUFFLE2(1, 0) ))};
		}
		
		ND_ Self  Floor ()							C_NE___	{ return Self{ _mm_floor_pd( _value )}; }
		ND_ Self  Ceil ()							C_NE___	{ return Self{ _mm_ceil_pd( _value )}; }
		template <int Mode> ND_ Self  Round ()		C_NE___	{ return Self{ _mm_round_pd( _value, Mode )}; }
	  #endif
		
		ND_ Self  Abs ()							C_NE___
		{
			__m128d	signmask = _mm_set1_pd( -0.0 );
			return Self{ _mm_andnot_pd( signmask, _value )};
		}

		// TODO: _mm_cmp_pd AVX
		
		ND_ Self  Reciporal ()						C_NE___	{ return Self{ _mm_rcp14_pd( _value )}; }							// 1 / x
		ND_ Self  Sqrt ()							C_NE___	{ return Self{ _mm_sqrt_pd( _value )}; }
	//	ND_ Self  RSqrt ()							C_NE___	{ return Self{ _mm_rsqrt14_pd( _value )}; }							// 1 / sqrt(x)
	//	ND_ Self  FastSqrt ()						C_NE___	{ return Self{ _mm_mul_pd( _value, _mm_rsqrt14_pd( _value ))}; }	// x / sqrt(x)
		ND_ Self  Scale (const Self &rhs)			C_NE___	{ return Self{ _mm_scalef_pd( _value, rhs._value )}; }				// a * 2^b
		
		ND_ Self  FMAdd (const Self &b, const Self &c)		C_NE___ { return Self{ _mm_fmadd_pd(    _value, b._value, c._value )}; }	// a * b + c
		ND_ Self  FMSub (const Self &b, const Self &c)		C_NE___ { return Self{ _mm_fmsub_pd(    _value, b._value, c._value )}; }	// a * b - c
		ND_ Self  FMAddSub (const Self &b, const Self &c)	C_NE___ { return Self{ _mm_fmaddsub_pd( _value, b._value, c._value )}; }	// { a0 * b0 - c0, a1 * b1 + c1, a2 * b2 - c2, a3 * b3 + c3 }
		ND_ Self  FMSubAdd (const Self &b, const Self &c)	C_NE___ { return Self{ _mm_fmaddsub_pd( _value, b._value, c._value )}; }	// { a0 * b0 + c0, a1 * b1 - c1, a2 * b2 + c2, a3 * b3 - c3 }
		ND_ Self  FNMAdd (const Self &b, const Self &c)		C_NE___ { return Self{ _mm_fnmadd_pd(   _value, b._value, c._value )}; }	// -a * b + c
		ND_ Self  FNMSub (const Self &b, const Self &c)		C_NE___ { return Self{ _mm_fnmsub_pd(   _value, b._value, c._value )}; }	// -a * b - c

			void  ToArray (OUT Value_t* dst)				C_NE___	{ _mm_storeu_pd( OUT dst, _value ); }
		
		template <typename IT>
		ND_ EnableIf< IsInteger<IT>, SimdTInt128<IT>>	Cast () C_NE___;
	};



	//
	// 128 bit int
	//
#	define AE_SIMD_Int128b
	struct Int128b
	{
	// types
	public:
		using Self	= Int128b;


	// variables
	private:
		__m128i		_value;

		
	// methods
	public:
		Int128b ()										__NE___	: _value{ _mm_setzero_si128() } {}
		explicit Int128b (int v)						__NE___	: _value{ _mm_cvtsi32_si128( v )} {}
		explicit Int128b (slong v)						__NE___	: _value{ _mm_cvtsi64_si128( v )} {}
		explicit Int128b (const __m128i &v)				__NE___	: _value{ v } {}
		
		ND_ Self	operator & (const Self &rhs)		C_NE___	{ return And( rhs ); }
		ND_ Self	operator | (const Self &rhs)		C_NE___	{ return Or( rhs ); }
		ND_ Self	operator ^ (const Self &rhs)		C_NE___	{ return Xor( rhs ); }

		ND_ Self	And (const Self &rhs)				C_NE___	{ return Self{ _mm_and_si128( _value, rhs._value )}; }
		ND_ Self	Or  (const Self &rhs)				C_NE___	{ return Self{ _mm_or_si128( _value, rhs._value )}; }
		ND_ Self	Xor (const Self &rhs)				C_NE___	{ return Self{ _mm_xor_si128( _value, rhs._value )}; }
		ND_ Self	AndNot (const Self &rhs)			C_NE___	{ return Self{ _mm_andnot_si128( _value, rhs._value )}; } // !a & b
		
		// shift in bytes
		template <int ShiftBytes> ND_ Self  LShiftB ()	C_NE___	{ return Self{ _mm_bslli_si128( _value, ShiftBytes )}; }
		template <int ShiftBytes> ND_ Self  RShiftB ()	C_NE___	{ return Self{ _mm_bsrli_si128( _value, ShiftBytes )}; }

		ND_ int		ToInt32 ()							C_NE___	{ return _mm_cvtsi128_si32( _value ); }
		ND_ slong	ToInt64 ()							C_NE___	{ return _mm_cvtsi128_si64( _value ); }
	};



	//
	// 128 bit integer (SSE 2.x)
	//
#	define AE_SIMD_SimdTInt128
	template <typename IntType>
	struct SimdTInt128
	{
		STATIC_ASSERT( IsInteger<IntType> );

	// types
	public:
		using Value_t	= IntType;
		using Self		= SimdTInt128< IntType >;
		using Bool128b	= SimdTInt128< IntType >;
		using Native_t	= __m128i;


	// variables
	private:
		Native_t	_value;		// int64[2], int32[4], int16[8], int8[16]

		static constexpr bool	is8		= sizeof( IntType ) == 1;
		static constexpr bool	is16	= sizeof( IntType ) == 2;
		static constexpr bool	is32	= sizeof( IntType ) == 4;
		static constexpr bool	is64	= sizeof( IntType ) == 8;
		
		static constexpr bool	isU8	= IsSameTypes< IntType, ubyte >;
		static constexpr bool	isU16	= IsSameTypes< IntType, ushort >;
		static constexpr bool	isU32	= IsSameTypes< IntType, uint >;
		static constexpr bool	isU64	= IsSameTypes< IntType, ulong >;
		
		static constexpr bool	isI8	= IsSameTypes< IntType, sbyte >;
		static constexpr bool	isI16	= IsSameTypes< IntType, sshort >;
		static constexpr bool	isI32	= IsSameTypes< IntType, sint >;
		static constexpr bool	isI64	= IsSameTypes< IntType, slong >;

		static constexpr uint	count	= sizeof(_value) / sizeof(IntType);


	// methods
	public:
		SimdTInt128 ()									__NE___	: _value{ _mm_setzero_si128() } {}
		explicit SimdTInt128 (const Native_t &v)		__NE___	: _value{ v } {}

		explicit SimdTInt128 (IntType v)				__NE___
		{
			if constexpr( is8 )		_value = _mm_set1_epi8( v );	else
			if constexpr( is16 )	_value = _mm_set1_epi16( v );	else
			if constexpr( is32 )	_value = _mm_set1_epi32( v );	else
			if constexpr( is64 )	_value = _mm_set1_epi64x( v );
		}
		
		explicit SimdTInt128 (Base::_hidden_::_UMax)	__NE___	:
			SimdTInt128{ ~IntType{0} }
		{}

		template <typename = EnableIf<is8>>
		SimdTInt128 (IntType v00, IntType v01, IntType v02, IntType v03,
					 IntType v04, IntType v05, IntType v06, IntType v07,
					 IntType v08, IntType v09, IntType v10, IntType v11,
					 IntType v12, IntType v13, IntType v14, IntType v15) __NE___ :
			_value{ _mm_set_epi8( v00, v01, v02, v03, v04, v05, v06, v07,
								  v08, v09, v10, v11, v12, v13, v14, v15 )} {}

		template <typename = EnableIf<is16>>
		SimdTInt128 (IntType v0, IntType v1, IntType v2, IntType v3,
					 IntType v4, IntType v5, IntType v6, IntType v7) __NE___ :
			_value{ _mm_set_epi16( v0, v1, v2, v3, v4, v5, v6, v7 )} {}

		template <typename = EnableIf<is32>>
		SimdTInt128 (IntType v0, IntType v1, IntType v2, IntType v3) __NE___ :
			_value{ _mm_set_epi32( v0, v1, v2, v3 )} {}
		
		template <typename = EnableIf<is64>>
		SimdTInt128 (IntType v0, IntType v1)			__NE___ :
			_value{ _mm_set_epi64x( v0, v1 )} {}

		ND_ Self	operator - ()						C_NE___	{ return Negative(); }
		ND_ Self	operator ~ ()						C_NE___	{ return Not(); }

		ND_ Self	operator + (const Self &rhs)		C_NE___	{ return Add( rhs ); }
		ND_ Self	operator - (const Self &rhs)		C_NE___	{ return Sub( rhs ); }
		ND_ Self	operator * (const Self &rhs)		C_NE___	{ return Mul( rhs ); }
		ND_ Self	operator / (const Self &rhs)		C_NE___	{ return Div( rhs ); }
		
		ND_ Self	operator & (const Self &rhs)		C_NE___	{ return And( rhs ); }
		ND_ Self	operator | (const Self &rhs)		C_NE___	{ return Or( rhs ); }
		ND_ Self	operator ^ (const Self &rhs)		C_NE___	{ return Xor( rhs ); }
		
		ND_ Self	operator << (int shift)				C_NE___	{ return LShift( shift ); }
		ND_ Self	operator << (const Self &rhs)		C_NE___	{ return LShift( rhs ); }
		ND_ Self	operator >> (int shift)				C_NE___	{ return RShift( shift ); }
		ND_ Self	operator >> (const Self &rhs)		C_NE___	{ return RShift( rhs ); }

		ND_ Bool128b  operator == (const Self &rhs)		C_NE___	{ return Equal( rhs ); }
		ND_ Bool128b  operator != (const Self &rhs)		C_NE___	{ return Equal( rhs ).Not(); }
		ND_ Bool128b  operator >  (const Self &rhs)		C_NE___	{ return Greater( rhs ); }
		ND_ Bool128b  operator <  (const Self &rhs)		C_NE___	{ return Less( rhs ); }
		ND_ Bool128b  operator >= (const Self &rhs)		C_NE___	{ return Less( rhs ).Not(); }
		ND_ Bool128b  operator <= (const Self &rhs)		C_NE___	{ return Greater( rhs ).Not(); }
		

		ND_ auto	operator [] (usize i)				C_NE___
		{
			ASSERT( i < count );
			if constexpr( isI8 )	return _value.m128i_i8[i];
			if constexpr( isU8 )	return _value.m128i_u8[i];
			if constexpr( isI16 )	return _value.m128i_i16[i];
			if constexpr( isU16 )	return _value.m128i_u16[i];
			if constexpr( isI32 )	return _value.m128i_i32[i];
			if constexpr( isU32 )	return _value.m128i_u32[i];
			if constexpr( isI64 )	return _value.m128i_i64[i];
			if constexpr( isU64 )	return _value.m128i_u64[i];
		}
		
		ND_ Native_t const&	Get ()						C_NE___	{ return _value; }

		ND_ Self	Negative ()							C_NE___	{ return Self{0}.Sub( *this ); }		// TODO: optimize ?

		ND_ Self	Add (const Self &rhs)				C_NE___
		{
			if constexpr( is8 )		return Self{ _mm_add_epi8(  _value, rhs._value )};
			if constexpr( is16 )	return Self{ _mm_add_epi16( _value, rhs._value )};
			if constexpr( is32 )	return Self{ _mm_add_epi32( _value, rhs._value )};
			if constexpr( is64 )	return Self{ _mm_add_epi64( _value, rhs._value )};
		}
		
		ND_ Self	AddSaturate (const Self &rhs)		C_NE___
		{
			if constexpr( isI8 )	return Self{ _mm_adds_epi8(  _value, rhs._value )};
			if constexpr( isU8 )	return Self{ _mm_adds_epu8(  _value, rhs._value )};
			if constexpr( isI16 )	return Self{ _mm_adds_epi16( _value, rhs._value )};
			if constexpr( isU16 )	return Self{ _mm_adds_epu16( _value, rhs._value )};
		}

		ND_ Self	Sub (const Self &rhs)				C_NE___
		{
			if constexpr( is8 )		return Self{ _mm_sub_epi8(  _value, rhs._value )};
			if constexpr( is16 )	return Self{ _mm_sub_epi16( _value, rhs._value )};
			if constexpr( is32 )	return Self{ _mm_sub_epi32( _value, rhs._value )};
			if constexpr( is64 )	return Self{ _mm_sub_epi64( _value, rhs._value )};
		}
		
		ND_ Self	SubSaturate (const Self &rhs)		C_NE___
		{
			if constexpr( isI8 )	return Self{ _mm_subs_epi8(  _value, rhs._value )};
			if constexpr( isU8 )	return Self{ _mm_subs_epu8(  _value, rhs._value )};
			if constexpr( isI16 )	return Self{ _mm_subs_epi16( _value, rhs._value )};
			if constexpr( isU16 )	return Self{ _mm_subs_epu16( _value, rhs._value )};
		}

		ND_ Self	Mul (const Self &rhs)				C_NE___
		{
			if constexpr( isU32 )	return Self{ _mm_mul_epu32( _value, rhs._value )};
		}
		
		ND_ Self	Not ()								C_NE___	{ return AndNot( Self{UMax} ); }
		ND_ Self	And (const Self &rhs)				C_NE___	{ return Self{ _mm_and_si128( _value, rhs._value )}; }
		ND_ Self	Or  (const Self &rhs)				C_NE___	{ return Self{ _mm_or_si128( _value, rhs._value )}; }
		ND_ Self	Xor (const Self &rhs)				C_NE___	{ return Self{ _mm_xor_si128( _value, rhs._value )}; }
		ND_ Self	AndNot (const Self &rhs)			C_NE___	{ return Self{ _mm_andnot_si128( _value, rhs._value )}; }	// ~a & b
		
		ND_ Self	Min (const Self &rhs)				C_NE___
		{
			if constexpr( isI8 )	return Self{ _mm_min_epi8(  _value, rhs._value )};
			if constexpr( isU8 )	return Self{ _mm_min_epu8(  _value, rhs._value )};
			if constexpr( isI16 )	return Self{ _mm_min_epi16( _value, rhs._value )};
			if constexpr( isU16 )	return Self{ _mm_min_epu16( _value, rhs._value )};
			if constexpr( isI32 )	return Self{ _mm_min_epi32( _value, rhs._value )};
			if constexpr( isU32 )	return Self{ _mm_min_epu32( _value, rhs._value )};
			if constexpr( isI64 )	return Self{ _mm_min_epi64( _value, rhs._value )};
			if constexpr( isU64 )	return Self{ _mm_min_epu64( _value, rhs._value )};
		}
		
		ND_ Self	Max (const Self &rhs)				C_NE___
		{
			if constexpr( isI8 )	return Self{ _mm_max_epi8(  _value, rhs._value )};
			if constexpr( isU8 )	return Self{ _mm_max_epu8(  _value, rhs._value )};
			if constexpr( isI16 )	return Self{ _mm_max_epi16( _value, rhs._value )};
			if constexpr( isU16 )	return Self{ _mm_max_epu16( _value, rhs._value )};
			if constexpr( isI32 )	return Self{ _mm_max_epi32( _value, rhs._value )};
			if constexpr( isU32 )	return Self{ _mm_max_epu32( _value, rhs._value )};
			if constexpr( isI64 )	return Self{ _mm_max_epi64( _value, rhs._value )};
			if constexpr( isU64 )	return Self{ _mm_max_epu64( _value, rhs._value )};
		}
		
		ND_ Self	Equal (const Self &rhs)				C_NE___
		{
			if constexpr( is8 )		return Self{ _mm_cmpeq_epi8(  _value, rhs._value )};
			if constexpr( is16 )	return Self{ _mm_cmpeq_epi16( _value, rhs._value )};
			if constexpr( is32 )	return Self{ _mm_cmpeq_epi32( _value, rhs._value )};
			if constexpr( is64 )	return Self{ _mm_cmpeq_epi64( _value, rhs._value )};
		}
		
		ND_ Self	Greater (const Self &rhs)			C_NE___
		{
			if constexpr( isI8 )	return Self{ _mm_cmpgt_epi8(  _value, rhs._value )};
			if constexpr( isI16 )	return Self{ _mm_cmpgt_epi16( _value, rhs._value )};
			if constexpr( isI32 )	return Self{ _mm_cmpgt_epi32( _value, rhs._value )};
			if constexpr( isI64 )	return Self{ _mm_cmpgt_epi64( _value, rhs._value )};
		}
		
		ND_ Self	Less (const Self &rhs)				C_NE___
		{
			if constexpr( isI8 )	return Self{ _mm_cmplt_epi8(  _value, rhs._value )};
			if constexpr( isI16 )	return Self{ _mm_cmplt_epi16( _value, rhs._value )};
			if constexpr( isI32 )	return Self{ _mm_cmplt_epi32( _value, rhs._value )};
		}
		
		// zero on overflow
		ND_ Self	LShift (int shift)					C_NE___
		{
			if constexpr( is16 )	return Self{ _mm_slli_epi16( _value, shift )};
			if constexpr( is32 )	return Self{ _mm_slli_epi32( _value, shift )};
			if constexpr( is64 )	return Self{ _mm_slli_epi64( _value, shift )};
		}

		ND_ Self	LShift (const Self &rhs)			C_NE___
		{
			if constexpr( is16 )	return Self{ _mm_sll_epi16( _value, rhs._value )};
			if constexpr( is32 )	return Self{ _mm_sll_epi32( _value, rhs._value )};
			if constexpr( is64 )	return Self{ _mm_sll_epi64( _value, rhs._value )};
		}

		// zero on overflow
		ND_ Self	RShift (int shift)					C_NE___
		{
			if constexpr( is16 )	return Self{ _mm_srli_epi16( _value, shift )};
			if constexpr( is32 )	return Self{ _mm_srli_epi32( _value, shift )};
			if constexpr( is64 )	return Self{ _mm_srli_epi64( _value, shift )};
		}

		ND_ Self	RShift (const Self &rhs)			C_NE___
		{
			if constexpr( is16 )	return Self{ _mm_srl_epi16( _value, rhs._value )};
			if constexpr( is32 )	return Self{ _mm_srl_epi32( _value, rhs._value )};
			if constexpr( is64 )	return Self{ _mm_srl_epi64( _value, rhs._value )};
		}
		
		// max on overflow
		ND_ Self	RShiftA (int shift)					C_NE___		// AVX512F + AVX512VL
		{
			if constexpr( is16 )	return Self{ _mm_srai_epi16( _value, shift )};
			if constexpr( is32 )	return Self{ _mm_srai_epi32( _value, shift )};
			if constexpr( is64 )	return Self{ _mm_srai_epi64( _value, shift )};
		}

		ND_ Self	RShiftA (const Self &rhs)			C_NE___		// AVX512F + AVX512VL
		{
			if constexpr( is16 )	return Self{ _mm_sra_epi16( _value, rhs._value )};
			if constexpr( is32 )	return Self{ _mm_sra_epi32( _value, rhs._value )};
			if constexpr( is64 )	return Self{ _mm_sra_epi64( _value, rhs._value )};
		}

		
		void  ToArray (OUT IntType* dst)				C_NE___
		{
			if constexpr( is8 )		_mm_storeu_epi8( OUT static_cast<void*>(dst), _value );
			if constexpr( is16 )	_mm_storeu_epi16( OUT static_cast<void*>(dst), _value );
			if constexpr( is32 )	_mm_storeu_epi32( OUT static_cast<void*>(dst), _value );
			if constexpr( is64 )	_mm_storeu_epi64( OUT static_cast<void*>(dst), _value );
		}

		// non SIMD
		ND_ bool  All ()								C_NE___
		{
			if constexpr( is8 )		{ IntType accum = UMax;  for (uint i = 0; i < count; ++i) accum &= _value.m128i_u8[i];   return accum == UMax; }
			if constexpr( is16 )	{ IntType accum = UMax;  for (uint i = 0; i < count; ++i) accum &= _value.m128i_u16[i];  return accum == UMax; }
			if constexpr( is32 )	return (_value.m128i_u32[0] & _value.m128i_u32[1] & _value.m128i_u32[2] & _value.m128i_u32[3]) == UMax;
			if constexpr( is64 )	return (_value.m128i_u64[0] & _value.m128i_u64[1]) == UMax;
		}
		
		ND_ bool  Any ()								C_NE___
		{
			if constexpr( is8 )		{ IntType accum = 0;  for (uint i = 0; i < count; ++i) accum |= _value.m128i_u8[i];   return accum == UMax; }
			if constexpr( is16 )	{ IntType accum = 0;  for (uint i = 0; i < count; ++i) accum |= _value.m128i_u16[i];  return accum == UMax; }
			if constexpr( is32 )	return (_value.m128i_u32[0] | _value.m128i_u32[1] | _value.m128i_u32[2] | _value.m128i_u32[3]) == UMax;
			if constexpr( is64 )	return (_value.m128i_u64[0] | _value.m128i_u64[1]) == UMax;
		}
		
		ND_ bool  None ()								C_NE___
		{
			if constexpr( is8 )		{ IntType accum = 0;  for (uint i = 0; i < count; ++i) accum |= _value.m128i_u8[i];   return accum == 0; }
			if constexpr( is16 )	{ IntType accum = 0;  for (uint i = 0; i < count; ++i) accum |= _value.m128i_u16[i];  return accum == 0; }
			if constexpr( is32 )	return (_value.m128i_u32[0] | _value.m128i_u32[1] | _value.m128i_u32[2] | _value.m128i_u32[3]) == 0;
			if constexpr( is64 )	return (_value.m128i_u64[0] | _value.m128i_u64[1]) == 0;
		}

			
		// TODO: AES

		template <typename IT>
		ND_ SimdTInt128<IT> &		Cast ()				__NE___	{ return reinterpret_cast< SimdTInt128<IT> &>(*this); }

		template <typename IT>
		ND_ SimdTInt128<IT> const&	Cast ()				C_NE___	{ return reinterpret_cast< SimdTInt128<IT> const &>(*this); }

		ND_ Int128b const&			Cast ()				C_NE___	{ return reinterpret_cast< Int128b const &>(*this); }
	};
	
	
	inline SimdFloat4::Bool4::Bool4 (const SimdInt4 &v)						__NE___	: _value{v.Get()} {}
	inline SimdFloat4::Bool4::Bool4 (const SimdUInt4 &v)					__NE___	: _value{v.Get()} {}

	template <typename IT>
	EnableIf< IsInteger<IT>, SimdTInt128<IT>>  SimdFloat4::Cast ()			C_NE___
	{
		return SimdTInt128<IT>{ _mm_castps_si128( _value )};
	}
	
	template <typename T>
	EnableIf< IsSameTypes<T, double>, SimdDouble2>  SimdFloat4::Cast ()		C_NE___
	{
		return SimdDouble2{ _mm_castps_pd( _value )};
	}
	

	inline SimdDouble2::Bool2::Bool2 (const SimdLong2 &v)					__NE___	: _value{v.Get()} {}
	inline SimdDouble2::Bool2::Bool2 (const SimdULong2 &v)					__NE___	: _value{v.Get()} {}

	template <typename IT>
	EnableIf< IsInteger<IT>, SimdTInt128<IT>>  SimdDouble2::Cast ()			C_NE___
	{
		return SimdTInt128<IT>{ _mm_castpd_si128( _value )};
	}


# endif // AE_SIMD_SSE

} // AE::Math
