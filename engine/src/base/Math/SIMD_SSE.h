// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
 SSE:
	https://software.intel.com/sites/landingpage/IntrinsicsGuide
	https://www.officedaytime.com/simd512e/
	ps    -- packed single precision
	pd    -- packed double precision
	epi32 -- packed 32-bit integers
	epu32 -- packed 32-bit unsigned integers
	epi64 -- packed 64-bit integers
*/

#pragma once

#include "base/Math/GLM.h"
#include "base/Math/Float16.h"


namespace AE::Math
{
# if (AE_SIMD_SSE > 0) or (AE_SIMD_AVX > 0)

	struct Int128b;
	struct SimdFloat4;
	struct SimdDouble2;
	template <typename IntType> struct SimdTInt128;
	using SimdByte16	= SimdTInt128< sbyte >;
	using SimdUByte16	= SimdTInt128< ubyte >;
	using SimdShort8	= SimdTInt128< sshort >;
	using SimdUShort8	= SimdTInt128< ushort >;
	using SimdInt4		= SimdTInt128< sint >;
	using SimdUInt4		= SimdTInt128< uint >;
	using SimdLong2		= SimdTInt128< slong >;
	using SimdULong2	= SimdTInt128< ulong >;

	struct Int256b;
	struct SimdFloat8;
	struct SimdDouble4;
	template <typename IntType>	struct SimdTInt256;
	using SimdByte32	= SimdTInt256< sbyte >;
	using SimdUByte32	= SimdTInt256< ubyte >;
	using SimdShort16	= SimdTInt256< sshort >;
	using SimdUShort16	= SimdTInt256< ushort >;
	using SimdInt8		= SimdTInt256< sint >;
	using SimdUInt8		= SimdTInt256< uint >;
	using SimdLong4		= SimdTInt256< slong >;
	using SimdULong4	= SimdTInt256< ulong >;
	using SimdInt128b2	= SimdTInt256< Int128b >;
	
	struct Int512b;
	struct SimdFloat16;
	struct SimdDouble8;
	template <typename IntType> struct SimdTInt512;
	using SimdByte64	= SimdTInt512< sbyte >;
	using SimdUByte64	= SimdTInt512< ubyte >;
	using SimdShort32	= SimdTInt512< sshort >;
	using SimdUShort32	= SimdTInt512< ushort >;
	using SimdInt16		= SimdTInt512< sint >;
	using SimdUInt16	= SimdTInt512< uint >;
	using SimdLong8		= SimdTInt512< slong >;
	using SimdULong8	= SimdTInt512< ulong >;
	using SimdInt128b4	= SimdTInt512< Int128b >;
	using SimdInt256b2	= SimdTInt512< Int256b >;
# endif
	
	
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
			explicit Bool4 (__m128 val) : _value{_mm_castps_si128(val)} {}
			explicit Bool4 (__m128i val) : _value{val} {}
			Bool4 (const SimdInt4 &v);
			Bool4 (const SimdUInt4 &v);

			ND_ Bool4  operator | (const Bool4 &rhs) const	{ return Bool4{ _mm_or_si128( _value, rhs._value )}; }
			ND_ Bool4  operator & (const Bool4 &rhs) const	{ return Bool4{ _mm_and_si128( _value, rhs._value )}; }
			ND_ Bool4  operator ^ (const Bool4 &rhs) const	{ return Bool4{ _mm_xor_si128( _value, rhs._value )}; }
			ND_ Bool4  operator ~ ()				 const	{ return Bool4{ _mm_andnot_si128( _value, _mm_set1_epi32(-1) )}; }
			ND_ bool   operator [] (usize i)		 const	{ ASSERT( i < 4 );  return _value.m128i_u32[i] == UMax; }

			ND_ bool  All ()	const	{ return (_value.m128i_u32[0] & _value.m128i_u32[1] & _value.m128i_u32[2] & _value.m128i_u32[3]) == UMax; }
			ND_ bool  Any ()	const	{ return (_value.m128i_u32[0] | _value.m128i_u32[1] | _value.m128i_u32[2] | _value.m128i_u32[3]) == UMax; }
			ND_ bool  None ()	const	{ return (_value.m128i_u32[0] | _value.m128i_u32[1] | _value.m128i_u32[2] | _value.m128i_u32[3]) == 0; }
		};


	// variables
	private:
		__m128	_value;		// float[4]


	// methods
	public:
		SimdFloat4 () : _value{ _mm_setzero_ps() } {}
		explicit SimdFloat4 (float v) : _value{ _mm_set1_ps( v )} {}
		explicit SimdFloat4 (const float* ptr) : _value{ _mm_loadu_ps( ptr )} { ASSERT( ptr != null ); }
		explicit SimdFloat4 (const __m128 &v) : _value{ v } {}
		SimdFloat4 (float x, float y, float z, float w) : _value{ _mm_set_ps( x, y, z, w )} {}

		ND_ Self  operator +  (const Self &rhs)	const	{ return Add( rhs ); }
		ND_ Self  operator -  (const Self &rhs)	const	{ return Sub( rhs ); }
		ND_ Self  operator *  (const Self &rhs)	const	{ return Mul( rhs ); }
		ND_ Self  operator /  (const Self &rhs)	const	{ return Div( rhs ); }
		
		ND_ Self  operator &  (const Self &rhs)	const	{ return And( rhs ); }
		ND_ Self  operator |  (const Self &rhs)	const	{ return Or( rhs ); }
		ND_ Self  operator ^  (const Self &rhs)	const	{ return Xor( rhs ); }

		ND_ Bool4  operator == (const Self &rhs)const	{ return Equal( rhs ); }
		ND_ Bool4  operator != (const Self &rhs)const	{ return NotEqual( rhs ); }
		ND_ Bool4  operator >  (const Self &rhs)const	{ return Greater( rhs ); }
		ND_ Bool4  operator <  (const Self &rhs)const	{ return Less( rhs ); }
		ND_ Bool4  operator >= (const Self &rhs)const	{ return GEqual( rhs ); }
		ND_ Bool4  operator <= (const Self &rhs)const	{ return LEqual( rhs ); }

		ND_ Value_t  operator [] (usize i)		const	{ ASSERT( i < 4 );  return _value.m128_f32[i]; }
		ND_ Value_t  GetX ()					const	{ return _mm_cvtss_f32( _value ); }

		ND_ __m128 const&  Get ()				const	{ return _value; }

		ND_ Self  Add (const Self &rhs)			const	{ return Self{ _mm_add_ps( _value, rhs._value )}; }
		ND_ Self  Sub (const Self &rhs)			const	{ return Self{ _mm_sub_ps( _value, rhs._value )}; }
		ND_ Self  Mul (const Self &rhs)			const	{ return Self{ _mm_mul_ps( _value, rhs._value )}; }
		ND_ Self  Div (const Self &rhs)			const	{ return Self{ _mm_div_ps( _value, rhs._value )}; }
		ND_ Self  Min (const Self &rhs)			const	{ return Self{ _mm_min_ps( _value, rhs._value )}; }
		ND_ Self  Max (const Self &rhs)			const	{ return Self{ _mm_max_ps( _value, rhs._value )}; }

		ND_ Self  And (const Self &rhs)			const	{ return Self{ _mm_and_ps( _value, rhs._value )}; }
		ND_ Self  Or  (const Self &rhs)			const	{ return Self{ _mm_or_ps( _value, rhs._value )}; }
		ND_ Self  Xor (const Self &rhs)			const	{ return Self{ _mm_xor_ps( _value, rhs._value )}; }
		ND_ Self  AndNot (const Self &rhs)		const	{ return Self{ _mm_andnot_ps( _value, rhs._value )}; }		// !a & b
		
	  #if AE_SIMD_SSE >= 30
		ND_ Self  HAdd (const Self &rhs)		const	{ return Self{ _mm_hadd_ps( _value, rhs._value )}; }		// { a0 + a1, a2 + a3, b0 + b1, b2 + b3 }
		ND_ Self  HSub (const Self &rhs)		const	{ return Self{ _mm_hsub_ps( _value, rhs._value )}; }		// { a0 - a1, a2 - a3, b0 - b1, b2 - b3 }

		ND_ Self  AddSub (const Self &rhs)		const	{ return Self{ _mm_addsub_ps( _value, rhs._value )}; }		// { a0 - b0, a1 + b1, a2 - b2, a3 + b3 }
	  #endif

		ND_ Self  FMAdd (const Self &b, const Self &c)		const { return Self{ _mm_fmadd_ps(    _value, b._value, c._value )}; }	// a * b + c
		ND_ Self  FMSub (const Self &b, const Self &c)		const { return Self{ _mm_fmsub_ps(    _value, b._value, c._value )}; }	// a * b - c
		ND_ Self  FMAddSub (const Self &b, const Self &c)	const { return Self{ _mm_fmaddsub_ps( _value, b._value, c._value )}; }	// { a0 * b0 - c0, a1 * b1 + c1, a2 * b2 - c2, a3 * b3 + c3 }
		ND_ Self  FMSubAdd (const Self &b, const Self &c)	const { return Self{ _mm_fmaddsub_ps( _value, b._value, c._value )}; }	// { a0 * b0 + c0, a1 * b1 - c1, a2 * b2 + c2, a3 * b3 - c3 }
		ND_ Self  FNMAdd (const Self &b, const Self &c)		const { return Self{ _mm_fnmadd_ps(   _value, b._value, c._value )}; }	// -a * b + c
		ND_ Self  FNMSub (const Self &b, const Self &c)		const { return Self{ _mm_fnmsub_ps(   _value, b._value, c._value )}; }	// -a * b - c

		ND_ Self  Sqrt ()						const	{ return Self{ _mm_sqrt_ps( _value )}; }
		ND_ Self  Reciporal ()					const	{ return Self{ _mm_rcp_ps( _value )}; }							// 1 / x
		ND_ Self  RSqrt ()						const	{ return Self{ _mm_rsqrt_ps( _value )}; }						// 1 / sqrt(x)
		ND_ Self  FastSqrt ()					const	{ return Self{ _mm_mul_ps( _value, _mm_rsqrt_ps( _value ))}; }	// x / sqrt(x)
		ND_ Self  Scale (const Self &rhs)		const	{ return Self{ _mm_scalef_ps( _value, rhs._value )}; }			// a * 2^b

		ND_ Self  Abs () const
		{
			__m128	signmask = _mm_set1_ps( -0.0f );
			return Self{ _mm_andnot_ps( signmask, _value )};
		}

		ND_ Self  AddScalar (const Self &rhs)	const	{ return Self{ _mm_add_ss( _value, rhs._value )}; }			// { a0 + b0, a1, a2, a3 }
		ND_ Self  SubScalar (const Self &rhs)	const	{ return Self{ _mm_sub_ss( _value, rhs._value )}; }			// { a0 - b0, a1, a2, a3 }
		ND_ Self  MulScalar (const Self &rhs)	const	{ return Self{ _mm_mul_ss( _value, rhs._value )}; }			// { a0 * b0, a1, a2, a3 }
		ND_ Self  DivScalar (const Self &rhs)	const	{ return Self{ _mm_div_ss( _value, rhs._value )}; }			// { a0 / b0, a1, a2, a3 }
		ND_ Self  SqrtScalar ()					const	{ return Self{ _mm_sqrt_ss( _value )}; }					// { sqrt(a0), a1, a2, a3 }
		ND_ Self  RSqrtScalar ()				const	{ return Self{ _mm_rsqrt_ss( _value )}; }					// { 1 / sqrt(a0), a1, a2, a3 }
		ND_ Self  ReciporalScalar ()			const	{ return Self{ _mm_rcp_ps( _value )}; }						// { 1 / a0, a1, a2, a3 }

		ND_ Bool4  Equal    (const Self &rhs)	const	{ return Bool4{ _mm_cmpeq_ps( _value, rhs._value )}; }
		ND_ Bool4  NotEqual (const Self &rhs)	const	{ return Bool4{ _mm_cmpneq_ps( _value, rhs._value )}; }
		ND_ Bool4  Greater  (const Self &rhs)	const	{ return Bool4{ _mm_cmpgt_ps( _value, rhs._value )}; }
		ND_ Bool4  Less     (const Self &rhs)	const	{ return Bool4{ _mm_cmplt_ps( _value, rhs._value )}; }
		ND_ Bool4  GEqual   (const Self &rhs)	const	{ return Bool4{ _mm_cmpge_ps( _value, rhs._value )}; }
		ND_ Bool4  LEqual   (const Self &rhs)	const	{ return Bool4{ _mm_cmple_ps( _value, rhs._value )}; }

	  #if AE_SIMD_AVX >= 1	// AVX 1
		// ordered - comparison with NaN returns false
		ND_ Bool4  EqualO    (const Self &rhs)	const	{ return Bool4{ _mm_cmp_ps( _value, rhs._value, _CMP_EQ_OQ  )}; }
		ND_ Bool4  NotEqualO (const Self &rhs)	const	{ return Bool4{ _mm_cmp_ps( _value, rhs._value, _CMP_NEQ_OQ )}; }
		ND_ Bool4  GreaterO  (const Self &rhs)	const	{ return Bool4{ _mm_cmp_ps( _value, rhs._value, _CMP_GT_OQ  )}; }
		ND_ Bool4  LessO     (const Self &rhs)	const	{ return Bool4{ _mm_cmp_ps( _value, rhs._value, _CMP_LT_OQ  )}; }
		ND_ Bool4  GEqualO   (const Self &rhs)	const	{ return Bool4{ _mm_cmp_ps( _value, rhs._value, _CMP_GE_OQ  )}; }
		ND_ Bool4  LEqualO   (const Self &rhs)	const	{ return Bool4{ _mm_cmp_ps( _value, rhs._value, _CMP_LE_OQ  )}; }
		
		// unordered - comparison with NaN returns true
		ND_ Bool4  EqualU    (const Self &rhs)	const	{ return Bool4{ _mm_cmp_ps( _value, rhs._value, _CMP_EQ_UQ  )}; }
		ND_ Bool4  NotEqualU (const Self &rhs)	const	{ return Bool4{ _mm_cmp_ps( _value, rhs._value, _CMP_NEQ_UQ )}; }
		ND_ Bool4  GreaterU  (const Self &rhs)	const	{ return Bool4{ _mm_cmp_ps( _value, rhs._value, _CMP_NLE_UQ )}; }
		ND_ Bool4  LessU     (const Self &rhs)	const	{ return Bool4{ _mm_cmp_ps( _value, rhs._value, _CMP_NGE_UQ )}; }
		ND_ Bool4  GEqualU   (const Self &rhs)	const	{ return Bool4{ _mm_cmp_ps( _value, rhs._value, _CMP_NLT_UQ )}; }
		ND_ Bool4  LEqualU   (const Self &rhs)	const	{ return Bool4{ _mm_cmp_ps( _value, rhs._value, _CMP_NGT_UQ )}; }
	  #endif

		// TODO: _mm_cmp_ps AVX
		
	  #if AE_SIMD_SSE >= 41
		ND_ Self  Floor ()						const	{ return Self{ _mm_floor_ps( _value )}; }
		ND_ Self  Ceil ()						const	{ return Self{ _mm_ceil_ps( _value )}; }

		template <int Mode> ND_ Self  Round ()	const	{ return Self{ _mm_round_ps( _value, Mode )}; }

		ND_ Self  Dot (const Self &rhs)			const	{ return Self{ _mm_dp_ps( _value, rhs._value, 0xFF )}; }
	  #endif

			void  ToArray (OUT Value_t* dst)	const	{ _mm_storeu_ps( OUT dst, _value ); }

		template <typename IT>
		ND_ EnableIf< IsInteger<IT>, SimdTInt128<IT>>  Cast () const;

		template <typename DT>
		ND_ EnableIf< IsSameTypes<DT, double>, SimdDouble2>  Cast () const;
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
			explicit Bool2 (__m128d val) : _value{_mm_castpd_si128(val)} {}
			explicit Bool2 (__m128i val) : _value{val} {}
			Bool2 (const SimdLong2 &v);
			Bool2 (const SimdULong2 &v);

			ND_ Bool2  operator | (const Bool2 &rhs) const	{ return Bool2{ _mm_or_si128( _value, rhs._value )}; }
			ND_ Bool2  operator & (const Bool2 &rhs) const	{ return Bool2{ _mm_and_si128( _value, rhs._value )}; }
			ND_ Bool2  operator ^ (const Bool2 &rhs) const	{ return Bool2{ _mm_xor_si128( _value, rhs._value )}; }
			ND_ Bool2  operator ~ ()				 const	{ return Bool2{ _mm_andnot_si128( _value, _mm_set1_epi64x(-1) )}; }
			ND_ bool   operator [] (usize i)		 const	{ ASSERT( i < 2 );  return _value.m128i_u64[i] == UMax; }

			ND_ bool  All ()	const	{ return (_value.m128i_u64[0] & _value.m128i_u64[1]) == UMax; }
			ND_ bool  Any ()	const	{ return (_value.m128i_u64[0] | _value.m128i_u64[1]) == UMax; }
			ND_ bool  None ()	const	{ return (_value.m128i_u64[0] | _value.m128i_u64[1]) == 0; }
		};


	// variables
	private:
		__m128d		_value;		// double[2]


	// methods
	public:
		SimdDouble2 () : _value{ _mm_setzero_pd() } {}
		explicit SimdDouble2 (double v) : _value{ _mm_set1_pd( v )} {}
		explicit SimdDouble2 (const double *v) : _value{ _mm_loadu_pd( v )} {}
		explicit SimdDouble2 (const __m128d &v) : _value{ v } {}
		SimdDouble2 (double x, double y) : _value{_mm_set_pd( x, y )} {}

		ND_ Self  operator +  (const Self &rhs)	const	{ return Add( rhs ); }
		ND_ Self  operator -  (const Self &rhs)	const	{ return Sub( rhs ); }
		ND_ Self  operator *  (const Self &rhs)	const	{ return Mul( rhs ); }
		ND_ Self  operator /  (const Self &rhs)	const	{ return Div( rhs ); }
		
		ND_ Self  operator &  (const Self &rhs)	const	{ return And( rhs ); }
		ND_ Self  operator |  (const Self &rhs)	const	{ return Or( rhs ); }
		ND_ Self  operator ^  (const Self &rhs)	const	{ return Xor( rhs ); }

		ND_ Bool2  operator == (const Self &rhs)const	{ return Equal( rhs ); }
		ND_ Bool2  operator != (const Self &rhs)const	{ return NotEqual( rhs ); }
		ND_ Bool2  operator >  (const Self &rhs)const	{ return Greater( rhs ); }
		ND_ Bool2  operator <  (const Self &rhs)const	{ return Less( rhs ); }
		ND_ Bool2  operator >= (const Self &rhs)const	{ return GEqual( rhs ); }
		ND_ Bool2  operator <= (const Self &rhs)const	{ return LEqual( rhs ); }
		
		ND_ Value_t  operator [] (usize i)		const	{ ASSERT( i < 2 );  return _value.m128d_f64[i]; }
		
		ND_ __m128d const&  Get ()				const	{ return _value; }


		ND_ Self  Add (const Self &rhs)			const	{ return Self{ _mm_add_pd( _value, rhs._value )}; }
		ND_ Self  Sub (const Self &rhs)			const	{ return Self{ _mm_sub_pd( _value, rhs._value )}; }
		ND_ Self  Mul (const Self &rhs)			const	{ return Self{ _mm_mul_pd( _value, rhs._value )}; }
		ND_ Self  Div (const Self &rhs)			const	{ return Self{ _mm_div_pd( _value, rhs._value )}; }
		ND_ Self  Min (const Self &rhs)			const	{ return Self{ _mm_min_pd( _value, rhs._value )}; }
		ND_ Self  Max (const Self &rhs)			const	{ return Self{ _mm_max_pd( _value, rhs._value )}; }
		
		ND_ Self  And (const Self &rhs)			const	{ return Self{ _mm_and_pd( _value, rhs._value )}; }
		ND_ Self  Or  (const Self &rhs)			const	{ return Self{ _mm_or_pd( _value, rhs._value )}; }
		ND_ Self  Xor (const Self &rhs)			const	{ return Self{ _mm_xor_pd( _value, rhs._value )}; }
		ND_ Self  AndNot (const Self &rhs)		const	{ return Self{ _mm_andnot_pd( _value, rhs._value )}; } // !a & b
		
	  #if AE_SIMD_SSE >= 30
		ND_ Self  HAdd (const Self &rhs)		const	{ return Self{ _mm_hadd_pd( _value, rhs._value )}; }	// { a0 + a1,  b0 + b1 }
		ND_ Self  HSub (const Self &rhs)		const	{ return Self{ _mm_hsub_pd( _value, rhs._value )}; }	// { a0 - a1,  b0 - b1 }
		
		ND_ Self  AddSub (const Self &rhs)		const	{ return Self{ _mm_addsub_pd( _value, rhs._value )}; }	// { a0 - b0, a1 + b1 }
	  #endif

		ND_ Bool2  Equal    (const Self &rhs)	const	{ return Bool2{ _mm_cmpeq_pd( _value, rhs._value )}; }
		ND_ Bool2  NotEqual (const Self &rhs)	const	{ return Bool2{ _mm_cmpneq_pd( _value, rhs._value )}; }
		ND_ Bool2  Greater  (const Self &rhs)	const	{ return Bool2{ _mm_cmpgt_pd( _value, rhs._value )}; }
		ND_ Bool2  Less     (const Self &rhs)	const	{ return Bool2{ _mm_cmplt_pd( _value, rhs._value )}; }
		ND_ Bool2  GEqual   (const Self &rhs)	const	{ return Bool2{ _mm_cmpge_pd( _value, rhs._value )}; }
		ND_ Bool2  LEqual   (const Self &rhs)	const	{ return Bool2{ _mm_cmple_pd( _value, rhs._value )}; }
		
	  #if AE_SIMD_AVX >= 1	// AVX 1
		// ordered - comparison with NaN returns false
		ND_ Bool2  EqualO    (const Self &rhs)	const	{ return Bool2{ _mm_cmp_pd( _value, rhs._value, _CMP_EQ_OQ  )}; }
		ND_ Bool2  NotEqualO (const Self &rhs)	const	{ return Bool2{ _mm_cmp_pd( _value, rhs._value, _CMP_NEQ_OQ )}; }
		ND_ Bool2  GreaterO  (const Self &rhs)	const	{ return Bool2{ _mm_cmp_pd( _value, rhs._value, _CMP_GT_OQ  )}; }
		ND_ Bool2  LessO     (const Self &rhs)	const	{ return Bool2{ _mm_cmp_pd( _value, rhs._value, _CMP_LT_OQ  )}; }
		ND_ Bool2  GEqualO   (const Self &rhs)	const	{ return Bool2{ _mm_cmp_pd( _value, rhs._value, _CMP_GE_OQ  )}; }
		ND_ Bool2  LEqualO   (const Self &rhs)	const	{ return Bool2{ _mm_cmp_pd( _value, rhs._value, _CMP_LE_OQ  )}; }
		
		// unordered - comparison with NaN returns true
		ND_ Bool2  EqualU    (const Self &rhs)	const	{ return Bool2{ _mm_cmp_pd( _value, rhs._value, _CMP_EQ_UQ  )}; }
		ND_ Bool2  NotEqualU (const Self &rhs)	const	{ return Bool2{ _mm_cmp_pd( _value, rhs._value, _CMP_NEQ_UQ )}; }
		ND_ Bool2  GreaterU  (const Self &rhs)	const	{ return Bool2{ _mm_cmp_pd( _value, rhs._value, _CMP_NLE_UQ )}; }
		ND_ Bool2  LessU     (const Self &rhs)	const	{ return Bool2{ _mm_cmp_pd( _value, rhs._value, _CMP_NGE_UQ )}; }
		ND_ Bool2  GEqualU   (const Self &rhs)	const	{ return Bool2{ _mm_cmp_pd( _value, rhs._value, _CMP_NLT_UQ )}; }
		ND_ Bool2  LEqualU   (const Self &rhs)	const	{ return Bool2{ _mm_cmp_pd( _value, rhs._value, _CMP_NGT_UQ )}; }
	  #endif

	  #if AE_SIMD_SSE >= 41
		ND_ Self  Dot (const Self &rhs)			const
		{
			return Self{ _mm_dp_pd( _value, rhs._value, 0xFF )};
			//__m128d	ab = _mm_mul_pd( _value, rhs._value );
			//return Self{ _mm_add_pd( ab, _mm_shuffle_pd( ab, ab, _MM_SHUFFLE2(1, 0) ))};
		}
		
		ND_ Self  Floor ()						const	{ return Self{ _mm_floor_pd( _value )}; }
		ND_ Self  Ceil ()						const	{ return Self{ _mm_ceil_pd( _value )}; }
		template <int Mode> ND_ Self  Round ()	const	{ return Self{ _mm_round_pd( _value, Mode )}; }
	  #endif
		
		ND_ Self  Abs () const
		{
			__m128d	signmask = _mm_set1_pd( -0.0 );
			return Self{ _mm_andnot_pd( signmask, _value )};
		}

		// TODO: _mm_cmp_pd AVX
		
		ND_ Self  Reciporal ()					const	{ return Self{ _mm_rcp14_pd( _value )}; }							// 1 / x
		ND_ Self  Sqrt ()						const	{ return Self{ _mm_sqrt_pd( _value )}; }
	//	ND_ Self  RSqrt ()						const	{ return Self{ _mm_rsqrt14_pd( _value )}; }							// 1 / sqrt(x)
	//	ND_ Self  FastSqrt ()					const	{ return Self{ _mm_mul_pd( _value, _mm_rsqrt14_pd( _value ))}; }	// x / sqrt(x)
		ND_ Self  Scale (const Self &rhs)		const	{ return Self{ _mm_scalef_pd( _value, rhs._value )}; }				// a * 2^b
		
		ND_ Self  FMAdd (const Self &b, const Self &c)		const { return Self{ _mm_fmadd_pd(    _value, b._value, c._value )}; }	// a * b + c
		ND_ Self  FMSub (const Self &b, const Self &c)		const { return Self{ _mm_fmsub_pd(    _value, b._value, c._value )}; }	// a * b - c
		ND_ Self  FMAddSub (const Self &b, const Self &c)	const { return Self{ _mm_fmaddsub_pd( _value, b._value, c._value )}; }	// { a0 * b0 - c0, a1 * b1 + c1, a2 * b2 - c2, a3 * b3 + c3 }
		ND_ Self  FMSubAdd (const Self &b, const Self &c)	const { return Self{ _mm_fmaddsub_pd( _value, b._value, c._value )}; }	// { a0 * b0 + c0, a1 * b1 - c1, a2 * b2 + c2, a3 * b3 - c3 }
		ND_ Self  FNMAdd (const Self &b, const Self &c)		const { return Self{ _mm_fnmadd_pd(   _value, b._value, c._value )}; }	// -a * b + c
		ND_ Self  FNMSub (const Self &b, const Self &c)		const { return Self{ _mm_fnmsub_pd(   _value, b._value, c._value )}; }	// -a * b - c

			void  ToArray (OUT Value_t* dst)	const	{ _mm_storeu_pd( OUT dst, _value ); }
		
		template <typename IT>
		ND_ EnableIf< IsInteger<IT>, SimdTInt128<IT>>	Cast () const;
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
		Int128b () : _value{ _mm_setzero_si128() } {}
		explicit Int128b (int v) : _value{ _mm_cvtsi32_si128( v )} {}
		explicit Int128b (slong v) : _value{ _mm_cvtsi64_si128( v )} {}
		explicit Int128b (const __m128i &v) : _value{ v } {}
		
		ND_ Self	operator & (const Self &rhs)	const	{ return And( rhs ); }
		ND_ Self	operator | (const Self &rhs)	const	{ return Or( rhs ); }
		ND_ Self	operator ^ (const Self &rhs)	const	{ return Xor( rhs ); }

		ND_ Self	And (const Self &rhs)			const	{ return Self{ _mm_and_si128( _value, rhs._value )}; }
		ND_ Self	Or  (const Self &rhs)			const	{ return Self{ _mm_or_si128( _value, rhs._value )}; }
		ND_ Self	Xor (const Self &rhs)			const	{ return Self{ _mm_xor_si128( _value, rhs._value )}; }
		ND_ Self	AndNot (const Self &rhs)		const	{ return Self{ _mm_andnot_si128( _value, rhs._value )}; } // !a & b
		
		// shift in bytes
		template <int ShiftBytes> ND_ Self  LShiftB ()	const	{ return Self{ _mm_bslli_si128( _value, ShiftBytes )}; }
		template <int ShiftBytes> ND_ Self  RShiftB ()	const	{ return Self{ _mm_bsrli_si128( _value, ShiftBytes )}; }

		ND_ int		ToInt32 ()						const	{ return _mm_cvtsi128_si32( _value ); }
		ND_ slong	ToInt64 ()						const	{ return _mm_cvtsi128_si64( _value ); }
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
		SimdTInt128 () : _value{ _mm_setzero_si128() } {}
		explicit SimdTInt128 (const Native_t &v) : _value{ v } {}

		explicit SimdTInt128 (IntType v)
		{
			if constexpr( is8 )		_value = _mm_set1_epi8( v );	else
			if constexpr( is16 )	_value = _mm_set1_epi16( v );	else
			if constexpr( is32 )	_value = _mm_set1_epi32( v );	else
			if constexpr( is64 )	_value = _mm_set1_epi64x( v );
		}
		
		explicit SimdTInt128 (Base::_hidden_::_UMax) :
			SimdTInt128{ ~IntType{0} }
		{}

		template <typename = EnableIf<is8>>
		SimdTInt128 (IntType v00, IntType v01, IntType v02, IntType v03,
					 IntType v04, IntType v05, IntType v06, IntType v07,
					 IntType v08, IntType v09, IntType v10, IntType v11,
					 IntType v12, IntType v13, IntType v14, IntType v15) :
			_value{ _mm_set_epi8( v00, v01, v02, v03, v04, v05, v06, v07,
								  v08, v09, v10, v11, v12, v13, v14, v15 )} {}

		template <typename = EnableIf<is16>>
		SimdTInt128 (IntType v0, IntType v1, IntType v2, IntType v3,
					 IntType v4, IntType v5, IntType v6, IntType v7) :
			_value{ _mm_set_epi16( v0, v1, v2, v3, v4, v5, v6, v7 )} {}

		template <typename = EnableIf<is32>>
		SimdTInt128 (IntType v0, IntType v1, IntType v2, IntType v3) :
			_value{ _mm_set_epi32( v0, v1, v2, v3 )} {}
		
		template <typename = EnableIf<is64>>
		SimdTInt128 (IntType v0, IntType v1) :
			_value{ _mm_set_epi64x( v0, v1 )} {}

		ND_ Self	operator - ()					const	{ return Negative(); }
		ND_ Self	operator ~ ()					const	{ return Not(); }

		ND_ Self	operator + (const Self &rhs)	const	{ return Add( rhs ); }
		ND_ Self	operator - (const Self &rhs)	const	{ return Sub( rhs ); }
		ND_ Self	operator * (const Self &rhs)	const	{ return Mul( rhs ); }
		ND_ Self	operator / (const Self &rhs)	const	{ return Div( rhs ); }
		
		ND_ Self	operator & (const Self &rhs)	const	{ return And( rhs ); }
		ND_ Self	operator | (const Self &rhs)	const	{ return Or( rhs ); }
		ND_ Self	operator ^ (const Self &rhs)	const	{ return Xor( rhs ); }
		
		ND_ Self	operator << (int shift)			const	{ return LShift( shift ); }
		ND_ Self	operator << (const Self &rhs)	const	{ return LShift( rhs ); }
		ND_ Self	operator >> (int shift)			const	{ return RShift( shift ); }
		ND_ Self	operator >> (const Self &rhs)	const	{ return RShift( rhs ); }

		ND_ Bool128b  operator == (const Self &rhs)	const	{ return Equal( rhs ); }
		ND_ Bool128b  operator != (const Self &rhs)	const	{ return Equal( rhs ).Not(); }
		ND_ Bool128b  operator >  (const Self &rhs)	const	{ return Greater( rhs ); }
		ND_ Bool128b  operator <  (const Self &rhs)	const	{ return Less( rhs ); }
		ND_ Bool128b  operator >= (const Self &rhs)	const	{ return Less( rhs ).Not(); }
		ND_ Bool128b  operator <= (const Self &rhs)	const	{ return Greater( rhs ).Not(); }
		

		ND_ auto	operator [] (usize i)	const
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
		
		ND_ Native_t const&	Get ()			const	{ return _value; }

		ND_ Self	Negative ()				const	{ return Self{0}.Sub( *this ); }		// TODO: optimize ?

		ND_ Self	Add (const Self &rhs)	const
		{
			if constexpr( is8 )		return Self{ _mm_add_epi8(  _value, rhs._value )};
			if constexpr( is16 )	return Self{ _mm_add_epi16( _value, rhs._value )};
			if constexpr( is32 )	return Self{ _mm_add_epi32( _value, rhs._value )};
			if constexpr( is64 )	return Self{ _mm_add_epi64( _value, rhs._value )};
		}
		
		ND_ Self	AddSaturate (const Self &rhs)	const
		{
			if constexpr( isI8 )	return Self{ _mm_adds_epi8(  _value, rhs._value )};
			if constexpr( isU8 )	return Self{ _mm_adds_epu8(  _value, rhs._value )};
			if constexpr( isI16 )	return Self{ _mm_adds_epi16( _value, rhs._value )};
			if constexpr( isU16 )	return Self{ _mm_adds_epu16( _value, rhs._value )};
		}

		ND_ Self	Sub (const Self &rhs)	const
		{
			if constexpr( is8 )		return Self{ _mm_sub_epi8(  _value, rhs._value )};
			if constexpr( is16 )	return Self{ _mm_sub_epi16( _value, rhs._value )};
			if constexpr( is32 )	return Self{ _mm_sub_epi32( _value, rhs._value )};
			if constexpr( is64 )	return Self{ _mm_sub_epi64( _value, rhs._value )};
		}
		
		ND_ Self	SubSaturate (const Self &rhs)	const
		{
			if constexpr( isI8 )	return Self{ _mm_subs_epi8(  _value, rhs._value )};
			if constexpr( isU8 )	return Self{ _mm_subs_epu8(  _value, rhs._value )};
			if constexpr( isI16 )	return Self{ _mm_subs_epi16( _value, rhs._value )};
			if constexpr( isU16 )	return Self{ _mm_subs_epu16( _value, rhs._value )};
		}

		ND_ Self	Mul (const Self &rhs)	const
		{
			if constexpr( isU32 )	return Self{ _mm_mul_epu32( _value, rhs._value )};
		}
		
		ND_ Self	Not ()					const	{ return AndNot( Self{UMax} ); }
		ND_ Self	And (const Self &rhs)	const	{ return Self{ _mm_and_si128( _value, rhs._value )}; }
		ND_ Self	Or  (const Self &rhs)	const	{ return Self{ _mm_or_si128( _value, rhs._value )}; }
		ND_ Self	Xor (const Self &rhs)	const	{ return Self{ _mm_xor_si128( _value, rhs._value )}; }
		ND_ Self	AndNot (const Self &rhs)const	{ return Self{ _mm_andnot_si128( _value, rhs._value )}; }	// ~a & b
		
		ND_ Self	Min (const Self &rhs)	const
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
		
		ND_ Self	Max (const Self &rhs)	const
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
		
		ND_ Self	Equal (const Self &rhs)		const
		{
			if constexpr( is8 )		return Self{ _mm_cmpeq_epi8(  _value, rhs._value )};
			if constexpr( is16 )	return Self{ _mm_cmpeq_epi16( _value, rhs._value )};
			if constexpr( is32 )	return Self{ _mm_cmpeq_epi32( _value, rhs._value )};
			if constexpr( is64 )	return Self{ _mm_cmpeq_epi64( _value, rhs._value )};
		}
		
		ND_ Self	Greater (const Self &rhs)	const
		{
			if constexpr( isI8 )	return Self{ _mm_cmpgt_epi8(  _value, rhs._value )};
			if constexpr( isI16 )	return Self{ _mm_cmpgt_epi16( _value, rhs._value )};
			if constexpr( isI32 )	return Self{ _mm_cmpgt_epi32( _value, rhs._value )};
			if constexpr( isI64 )	return Self{ _mm_cmpgt_epi64( _value, rhs._value )};
		}
		
		ND_ Self	Less (const Self &rhs)		const
		{
			if constexpr( isI8 )	return Self{ _mm_cmplt_epi8(  _value, rhs._value )};
			if constexpr( isI16 )	return Self{ _mm_cmplt_epi16( _value, rhs._value )};
			if constexpr( isI32 )	return Self{ _mm_cmplt_epi32( _value, rhs._value )};
		}
		
		// zero on overflow
		ND_ Self	LShift (int shift)			const
		{
			if constexpr( is16 )	return Self{ _mm_slli_epi16( _value, shift )};
			if constexpr( is32 )	return Self{ _mm_slli_epi32( _value, shift )};
			if constexpr( is64 )	return Self{ _mm_slli_epi64( _value, shift )};
		}

		ND_ Self	LShift (const Self &rhs)	const
		{
			if constexpr( is16 )	return Self{ _mm_sll_epi16( _value, rhs._value )};
			if constexpr( is32 )	return Self{ _mm_sll_epi32( _value, rhs._value )};
			if constexpr( is64 )	return Self{ _mm_sll_epi64( _value, rhs._value )};
		}

		// zero on overflow
		ND_ Self	RShift (int shift)			const
		{
			if constexpr( is16 )	return Self{ _mm_srli_epi16( _value, shift )};
			if constexpr( is32 )	return Self{ _mm_srli_epi32( _value, shift )};
			if constexpr( is64 )	return Self{ _mm_srli_epi64( _value, shift )};
		}

		ND_ Self	RShift (const Self &rhs)	const
		{
			if constexpr( is16 )	return Self{ _mm_srl_epi16( _value, rhs._value )};
			if constexpr( is32 )	return Self{ _mm_srl_epi32( _value, rhs._value )};
			if constexpr( is64 )	return Self{ _mm_srl_epi64( _value, rhs._value )};
		}
		
		// max on overflow
		ND_ Self	RShiftA (int shift)			const		// AVX512F + AVX512VL
		{
			if constexpr( is16 )	return Self{ _mm_srai_epi16( _value, shift )};
			if constexpr( is32 )	return Self{ _mm_srai_epi32( _value, shift )};
			if constexpr( is64 )	return Self{ _mm_srai_epi64( _value, shift )};
		}

		ND_ Self	RShiftA (const Self &rhs)	const		// AVX512F + AVX512VL
		{
			if constexpr( is16 )	return Self{ _mm_sra_epi16( _value, rhs._value )};
			if constexpr( is32 )	return Self{ _mm_sra_epi32( _value, rhs._value )};
			if constexpr( is64 )	return Self{ _mm_sra_epi64( _value, rhs._value )};
		}

		
		void  ToArray (OUT IntType* dst)		const
		{
			if constexpr( is8 )		_mm_storeu_epi8( OUT static_cast<void*>(dst), _value );
			if constexpr( is16 )	_mm_storeu_epi16( OUT static_cast<void*>(dst), _value );
			if constexpr( is32 )	_mm_storeu_epi32( OUT static_cast<void*>(dst), _value );
			if constexpr( is64 )	_mm_storeu_epi64( OUT static_cast<void*>(dst), _value );
		}

		// non SIMD
		ND_ bool  All () const
		{
			if constexpr( is8 )		{ IntType accum = UMax;  for (uint i = 0; i < count; ++i) accum &= _value.m128i_u8[i];   return accum == UMax; }
			if constexpr( is16 )	{ IntType accum = UMax;  for (uint i = 0; i < count; ++i) accum &= _value.m128i_u16[i];  return accum == UMax; }
			if constexpr( is32 )	return (_value.m128i_u32[0] & _value.m128i_u32[1] & _value.m128i_u32[2] & _value.m128i_u32[3]) == UMax;
			if constexpr( is64 )	return (_value.m128i_u64[0] & _value.m128i_u64[1]) == UMax;
		}
		
		ND_ bool  Any () const
		{
			if constexpr( is8 )		{ IntType accum = 0;  for (uint i = 0; i < count; ++i) accum |= _value.m128i_u8[i];   return accum == UMax; }
			if constexpr( is16 )	{ IntType accum = 0;  for (uint i = 0; i < count; ++i) accum |= _value.m128i_u16[i];  return accum == UMax; }
			if constexpr( is32 )	return (_value.m128i_u32[0] | _value.m128i_u32[1] | _value.m128i_u32[2] | _value.m128i_u32[3]) == UMax;
			if constexpr( is64 )	return (_value.m128i_u64[0] | _value.m128i_u64[1]) == UMax;
		}
		
		ND_ bool  None () const
		{
			if constexpr( is8 )		{ IntType accum = 0;  for (uint i = 0; i < count; ++i) accum |= _value.m128i_u8[i];   return accum == 0; }
			if constexpr( is16 )	{ IntType accum = 0;  for (uint i = 0; i < count; ++i) accum |= _value.m128i_u16[i];  return accum == 0; }
			if constexpr( is32 )	return (_value.m128i_u32[0] | _value.m128i_u32[1] | _value.m128i_u32[2] | _value.m128i_u32[3]) == 0;
			if constexpr( is64 )	return (_value.m128i_u64[0] | _value.m128i_u64[1]) == 0;
		}

			
		// TODO: AES

		template <typename IT>
		ND_ SimdTInt128<IT> &		Cast ()			{ return reinterpret_cast< SimdTInt128<IT> &>(*this); }

		template <typename IT>
		ND_ SimdTInt128<IT> const&	Cast ()	const	{ return reinterpret_cast< SimdTInt128<IT> const &>(*this); }

		ND_ Int128b const&			Cast ()	const	{ return reinterpret_cast< Int128b const &>(*this); }
	};
	
	
	inline SimdFloat4::Bool4::Bool4 (const SimdInt4 &v) : _value{v.Get()} {}
	inline SimdFloat4::Bool4::Bool4 (const SimdUInt4 &v) : _value{v.Get()} {}

	template <typename IT>
	EnableIf< IsInteger<IT>, SimdTInt128<IT>>  SimdFloat4::Cast () const
	{
		return SimdTInt128<IT>{ _mm_castps_si128( _value )};
	}
	
	template <typename T>
	EnableIf< IsSameTypes<T, double>, SimdDouble2>  SimdFloat4::Cast () const
	{
		return SimdDouble2{ _mm_castps_pd( _value )};
	}
	

	inline SimdDouble2::Bool2::Bool2 (const SimdLong2 &v) : _value{v.Get()} {}
	inline SimdDouble2::Bool2::Bool2 (const SimdULong2 &v) : _value{v.Get()} {}

	template <typename IT>
	EnableIf< IsInteger<IT>, SimdTInt128<IT>>  SimdDouble2::Cast () const
	{
		return SimdTInt128<IT>{ _mm_castpd_si128( _value )};
	}

# endif // AE_SIMD_SSE
//-----------------------------------------------------------------------------


# if AE_SIMD_AVX >= 1

	//
	// 256 bits float (AVX)
	//
	struct SimdFloat8
	{
	// types
	public:
		using Value_t	= float;
		using Self		= SimdFloat8;
		
		struct Bool8
		{
		private:
			__m256i		_value;		// uint[8]

		public:
			explicit Bool8 (const __m256i &val) : _value{val} {}
			explicit Bool8 (const __m256 &val) : _value{_mm256_castps_si256(val)} {}
			
			ND_ Bool8  operator | (const Bool8 &rhs)	const	{ return Bool8{ _mm256_or_si256(  _value, rhs._value )}; }
			ND_ Bool8  operator & (const Bool8 &rhs)	const	{ return Bool8{ _mm256_and_si256( _value, rhs._value )}; }
			ND_ Bool8  operator ^ (const Bool8 &rhs)	const	{ return Bool8{ _mm256_xor_si256( _value, rhs._value )}; }
			ND_ Bool8  operator ~ ()					const	{ return Bool8{ _mm256_andnot_si256( _value, _mm256_set1_epi32(-1) )}; }
			ND_ bool  operator [] (usize i)				const	{ ASSERT( i < 8 );  return _value.m256i_u32[i] == UMax; }

			ND_ bool  All ()	const
			{
				return (_value.m256i_u32[0] & _value.m256i_u32[1] & _value.m256i_u32[2] & _value.m256i_u32[3] &
						_value.m256i_u32[4] & _value.m256i_u32[5] & _value.m256i_u32[6] & _value.m256i_u32[7]) == UMax;
			}

			ND_ bool  Any ()	const
			{
				return (_value.m256i_u32[0] | _value.m256i_u32[1] | _value.m256i_u32[2] | _value.m256i_u32[3] |
						_value.m256i_u32[4] | _value.m256i_u32[5] | _value.m256i_u32[6] | _value.m256i_u32[7]) == UMax;
			}

			ND_ bool  None ()	const
			{
				return (_value.m256i_u32[0] | _value.m256i_u32[1] | _value.m256i_u32[2] | _value.m256i_u32[3] |
						_value.m256i_u32[4] | _value.m256i_u32[5] | _value.m256i_u32[6] | _value.m256i_u32[7]) == 0;
			}
		};


	// variables
	private:
		__m256		_value;		// float[8]


	// methods
	public:
		SimdFloat8 () : _value{ _mm256_setzero_ps() } {}
		explicit SimdFloat8 (float v) : _value{ _mm256_set1_ps( v )} {}
		explicit SimdFloat8 (const float* ptr) : _value{ _mm256_loadu_ps( ptr )} { ASSERT( ptr != null ); }
		explicit SimdFloat8 (const __m256 &v) : _value{v} {}

		SimdFloat8 (float v0, float v1, float v2, float v3,
					float v4, float v5, float v6, float v7) :
			_value{ _mm256_set_ps( v0, v1, v2, v3, v4, v5, v6, v7 )} {}

		ND_ Self  operator +  (const Self &rhs)	const	{ return Add( rhs ); }
		ND_ Self  operator -  (const Self &rhs)	const	{ return Sub( rhs ); }
		ND_ Self  operator *  (const Self &rhs)	const	{ return Mul( rhs ); }
		ND_ Self  operator /  (const Self &rhs)	const	{ return Div( rhs ); }
		
		ND_ Self  operator &  (const Self &rhs)	const	{ return And( rhs ); }
		ND_ Self  operator |  (const Self &rhs)	const	{ return Or( rhs ); }
		ND_ Self  operator ^  (const Self &rhs)	const	{ return Xor( rhs ); }

		ND_ Bool8  operator == (const Self &rhs)const	{ return Equal( rhs ); }
		ND_ Bool8  operator != (const Self &rhs)const	{ return NotEqual( rhs ); }
		ND_ Bool8  operator >  (const Self &rhs)const	{ return Greater( rhs ); }
		ND_ Bool8  operator <  (const Self &rhs)const	{ return Less( rhs ); }
		ND_ Bool8  operator >= (const Self &rhs)const	{ return GEqual( rhs ); }
		ND_ Bool8  operator <= (const Self &rhs)const	{ return LEqual( rhs ); }

		ND_ Value_t  operator [] (usize i)		const	{ ASSERT( i < 8 ); return _value.m256_f32[i]; }
		ND_ Value_t  GetX ()					const	{ return _mm256_cvtss_f32( _value ); }
		
		ND_ __m256 const&  Get ()				const	{ return _value; }

		ND_ Self  Add (const Self &rhs)			const	{ return Self{ _mm256_add_ps( _value, rhs._value )}; }
		ND_ Self  Sub (const Self &rhs)			const	{ return Self{ _mm256_sub_ps( _value, rhs._value )}; }
		ND_ Self  Mul (const Self &rhs)			const	{ return Self{ _mm256_mul_ps( _value, rhs._value )}; }
		ND_ Self  Div (const Self &rhs)			const	{ return Self{ _mm256_div_ps( _value, rhs._value )}; }
		ND_ Self  Min (const Self &rhs)			const	{ return Self{ _mm256_min_ps( _value, rhs._value )}; }
		ND_ Self  Max (const Self &rhs)			const	{ return Self{ _mm256_max_ps( _value, rhs._value )}; }
		
		ND_ Self  And (const Self &rhs)			const	{ return Self{ _mm256_and_ps( _value, rhs._value )}; }
		ND_ Self  Or  (const Self &rhs)			const	{ return Self{ _mm256_or_ps( _value, rhs._value )}; }
		ND_ Self  Xor (const Self &rhs)			const	{ return Self{ _mm256_xor_ps( _value, rhs._value )}; }
		ND_ Self  AndNot (const Self &rhs)		const	{ return Self{ _mm256_andnot_ps( _value, rhs._value )}; } // !a & b
		
		ND_ Self  HAdd (const Self &rhs)		const	{ return Self{ _mm256_hadd_ps( _value, rhs._value )}; }		// { a0 + a1, a2 + a3, b0 + b1, b2 + b3 }
		ND_ Self  HSub (const Self &rhs)		const	{ return Self{ _mm256_hsub_ps( _value, rhs._value )}; }		// { a0 - a1, a2 - a3, b0 - b1, b2 - b3 }
	
		ND_ Self  AddSub (const Self &rhs)		const	{ return Self{ _mm256_addsub_ps( _value, rhs._value )}; }	// { a0 - b0, a1 + b1, a2 - b2, a3 + b3 }
		
		ND_ Self  Abs () const
		{
			__m256	signmask = _mm256_set1_ps( -0.0f );
			return Self{ _mm256_andnot_ps( signmask, _value )};
		}

		ND_ Self  Reciporal ()					const	{ return Self{ _mm256_rcp_ps( _value )}; }								// 1 / x
		ND_ Self  Sqrt ()						const	{ return Self{ _mm256_sqrt_ps( _value )}; }
		ND_ Self  RSqrt ()						const	{ return Self{ _mm256_rsqrt_ps( _value )}; }							// 1 / sqrt(x)
		ND_ Self  FastSqrt ()					const	{ return Self{ _mm256_mul_ps( _value, _mm256_rsqrt_ps( _value ))}; }	// x / sqrt(x)
		
		// ordered - comparison with NaN returns false
		ND_ Bool8  Equal    (const Self &rhs)	const	{ return Bool8{ _mm256_cmp_ps( _value, rhs._value, _CMP_EQ_OQ  )}; }
		ND_ Bool8  NotEqual (const Self &rhs)	const	{ return Bool8{ _mm256_cmp_ps( _value, rhs._value, _CMP_NEQ_OQ )}; }
		ND_ Bool8  Greater  (const Self &rhs)	const	{ return Bool8{ _mm256_cmp_ps( _value, rhs._value, _CMP_GT_OQ  )}; }
		ND_ Bool8  Less     (const Self &rhs)	const	{ return Bool8{ _mm256_cmp_ps( _value, rhs._value, _CMP_LT_OQ  )}; }
		ND_ Bool8  GEqual   (const Self &rhs)	const	{ return Bool8{ _mm256_cmp_ps( _value, rhs._value, _CMP_GE_OQ  )}; }
		ND_ Bool8  LEqual   (const Self &rhs)	const	{ return Bool8{ _mm256_cmp_ps( _value, rhs._value, _CMP_LE_OQ  )}; }
		
		// unordered - comparison with NaN returns true
		ND_ Bool8  EqualU    (const Self &rhs)	const	{ return Bool8{ _mm256_cmp_ps( _value, rhs._value, _CMP_EQ_UQ  )}; }
		ND_ Bool8  NotEqualU (const Self &rhs)	const	{ return Bool8{ _mm256_cmp_ps( _value, rhs._value, _CMP_NEQ_UQ )}; }
		ND_ Bool8  GreaterU  (const Self &rhs)	const	{ return Bool8{ _mm256_cmp_ps( _value, rhs._value, _CMP_NLE_UQ )}; }
		ND_ Bool8  LessU     (const Self &rhs)	const	{ return Bool8{ _mm256_cmp_ps( _value, rhs._value, _CMP_NGE_UQ )}; }
		ND_ Bool8  GEqualU   (const Self &rhs)	const	{ return Bool8{ _mm256_cmp_ps( _value, rhs._value, _CMP_NLT_UQ )}; }
		ND_ Bool8  LEqualU   (const Self &rhs)	const	{ return Bool8{ _mm256_cmp_ps( _value, rhs._value, _CMP_NGT_UQ )}; }
		
		ND_ Self  Dot (const Self &rhs)			const	{ return Self{ _mm256_dp_ps( _value, rhs._value, 0xFF )}; }
		
		ND_ Self  Floor ()						const	{ return Self{ _mm256_floor_ps( _value )}; }
		ND_ Self  Ceil ()						const	{ return Self{ _mm256_ceil_ps( _value )}; }
		template <int Mode> ND_ Self  Round ()	const	{ return Self{ _mm256_round_ps( _value, Mode )}; }

		template <uint Idx> ND_ SimdFloat4  ToFloat4 () const { STATIC_ASSERT( Idx < 2 );  return SimdFloat4{ _mm256_extractf128_ps( _value, Idx )}; }

			void  ToArray (OUT Value_t* dst)	const	{ _mm256_storeu_ps( OUT dst, _value ); }
	};
	


	//
	// 256 bits double (AVX)
	//
	struct SimdDouble4
	{
	// types
	public:
		using Value_t	= double;
		using Self		= SimdDouble4;
		
		struct Bool4
		{
		private:
			__m256i		_value;		// ulong[8]

		public:
			explicit Bool4 (const __m256i &val) : _value{val} {}
			explicit Bool4 (const __m256d &val) : _value{_mm256_castpd_si256(val)} {}
			
			ND_ Bool4  operator | (const Bool4 &rhs)	const	{ return Bool4{ _mm256_or_si256(  _value, rhs._value )}; }
			ND_ Bool4  operator & (const Bool4 &rhs)	const	{ return Bool4{ _mm256_and_si256( _value, rhs._value )}; }
			ND_ Bool4  operator ^ (const Bool4 &rhs)	const	{ return Bool4{ _mm256_xor_si256( _value, rhs._value )}; }
			ND_ Bool4  operator ~ ()					const	{ return Bool4{ _mm256_andnot_si256( _value, _mm256_set1_epi64x(-1) )}; }
			ND_ bool  operator [] (usize i)				const	{ ASSERT( i < 4 );  return _value.m256i_u64[i] == UMax; }

			ND_ bool  All ()	const	{ return (_value.m256i_u64[0] & _value.m256i_u64[1] & _value.m256i_u64[2] & _value.m256i_u64[3]) == UMax; }
			ND_ bool  Any ()	const	{ return (_value.m256i_u64[0] | _value.m256i_u64[1] | _value.m256i_u64[2] | _value.m256i_u64[3]) == UMax; }
			ND_ bool  None ()	const	{ return (_value.m256i_u64[0] | _value.m256i_u64[1] | _value.m256i_u64[2] | _value.m256i_u64[3]) == 0; }
		};


	// variables
	private:
		__m256d		_value;		// double[4]


	// methods
	public:
		SimdDouble4 () : _value{ _mm256_setzero_pd() } {}
		explicit SimdDouble4 (double v) : _value{ _mm256_set1_pd( v )} {}
		explicit SimdDouble4 (const double *v) : _value{ _mm256_loadu_pd( v )} {}
		explicit SimdDouble4 (const __m256d &v) : _value{ v } {}
		SimdDouble4 (double x, double y, double z, double w) : _value{ _mm256_set_pd( x, y, z, w )} {}

		ND_ Self  operator + (const Self &rhs)	const	{ return Add( rhs ); }
		ND_ Self  operator - (const Self &rhs)	const	{ return Sub( rhs ); }
		ND_ Self  operator * (const Self &rhs)	const	{ return Mul( rhs ); }
		ND_ Self  operator / (const Self &rhs)	const	{ return Div( rhs ); }
		
		ND_ Self  operator &  (const Self &rhs)	const	{ return And( rhs ); }
		ND_ Self  operator |  (const Self &rhs)	const	{ return Or( rhs ); }
		ND_ Self  operator ^  (const Self &rhs)	const	{ return Xor( rhs ); }

		ND_ Bool4  operator == (const Self &rhs)const	{ return Equal( rhs ); }
		ND_ Bool4  operator != (const Self &rhs)const	{ return NotEqual( rhs ); }
		ND_ Bool4  operator >  (const Self &rhs)const	{ return Greater( rhs ); }
		ND_ Bool4  operator <  (const Self &rhs)const	{ return Less( rhs ); }
		ND_ Bool4  operator >= (const Self &rhs)const	{ return GEqual( rhs ); }
		ND_ Bool4  operator <= (const Self &rhs)const	{ return LEqual( rhs ); }
		
		ND_ Value_t  operator [] (usize i)		const	{ ASSERT( i < 4 ); return _value.m256d_f64[i]; }
		ND_ Value_t  GetX ()					const	{ return _mm256_cvtsd_f64( _value ); }
		
		ND_ __m256d const&  Get ()				const	{ return _value; }

		ND_ Self  Add (const Self &rhs)			const	{ return Self{ _mm256_add_pd( _value, rhs._value )}; }
		ND_ Self  Sub (const Self &rhs)			const	{ return Self{ _mm256_sub_pd( _value, rhs._value )}; }
		ND_ Self  Mul (const Self &rhs)			const	{ return Self{ _mm256_mul_pd( _value, rhs._value )}; }
		ND_ Self  Div (const Self &rhs)			const	{ return Self{ _mm256_div_pd( _value, rhs._value )}; }
		ND_ Self  Min (const Self &rhs)			const	{ return Self{ _mm256_min_pd( _value, rhs._value )}; }
		ND_ Self  Max (const Self &rhs)			const	{ return Self{ _mm256_max_pd( _value, rhs._value )}; }
		
		ND_ Self  And (const Self &rhs)			const	{ return Self{ _mm256_and_pd( _value, rhs._value )}; }
		ND_ Self  Or  (const Self &rhs)			const	{ return Self{ _mm256_or_pd( _value, rhs._value )}; }
		ND_ Self  Xor (const Self &rhs)			const	{ return Self{ _mm256_xor_pd( _value, rhs._value )}; }
		ND_ Self  AndNot (const Self &rhs)		const	{ return Self{ _mm256_andnot_pd( _value, rhs._value )}; } // !a & b

		// returns { a[0] + a[1],  a[2] + a[3],  b[0] + b[1],  b[2] + b[3] }
		ND_ Self  HAdd (const Self &rhs)		const	{ return Self{ _mm256_hadd_pd( _value, rhs._value )}; }
		
		// returns { a[0] - b[0], a[1] + b[1], a[2] - b[2], a[3] + b[3] }
		ND_ Self  AddSub (const Self &rhs)		const	{ return Self{ _mm256_addsub_pd( _value, rhs._value )}; }

		ND_ Self  Sqrt ()						const	{ return Self{ _mm256_sqrt_pd( _value )}; }
		
		ND_ Self  Abs () const
		{
			__m256d	signmask = _mm256_set1_pd( -0.0 );
			return Self{ _mm256_andnot_pd( signmask, _value )};
		}

		// ordered - comparison with NaN returns false
		ND_ Bool4  Equal    (const Self &rhs)	const	{ return Bool4{ _mm256_cmp_pd( _value, rhs._value, _CMP_EQ_OQ  )}; }
		ND_ Bool4  NotEqual (const Self &rhs)	const	{ return Bool4{ _mm256_cmp_pd( _value, rhs._value, _CMP_NEQ_OQ )}; }
		ND_ Bool4  Greater  (const Self &rhs)	const	{ return Bool4{ _mm256_cmp_pd( _value, rhs._value, _CMP_GT_OQ  )}; }
		ND_ Bool4  Less     (const Self &rhs)	const	{ return Bool4{ _mm256_cmp_pd( _value, rhs._value, _CMP_LT_OQ  )}; }
		ND_ Bool4  GEqual   (const Self &rhs)	const	{ return Bool4{ _mm256_cmp_pd( _value, rhs._value, _CMP_GE_OQ  )}; }
		ND_ Bool4  LEqual   (const Self &rhs)	const	{ return Bool4{ _mm256_cmp_pd( _value, rhs._value, _CMP_LE_OQ  )}; }
		
		// unordered - comparison with NaN returns true
		ND_ Bool4  EqualU    (const Self &rhs)	const	{ return Bool4{ _mm256_cmp_pd( _value, rhs._value, _CMP_EQ_UQ  )}; }
		ND_ Bool4  NotEqualU (const Self &rhs)	const	{ return Bool4{ _mm256_cmp_pd( _value, rhs._value, _CMP_NEQ_UQ )}; }
		ND_ Bool4  GreaterU  (const Self &rhs)	const	{ return Bool4{ _mm256_cmp_pd( _value, rhs._value, _CMP_NLE_UQ )}; }
		ND_ Bool4  LessU     (const Self &rhs)	const	{ return Bool4{ _mm256_cmp_pd( _value, rhs._value, _CMP_NGE_UQ )}; }
		ND_ Bool4  GEqualU   (const Self &rhs)	const	{ return Bool4{ _mm256_cmp_pd( _value, rhs._value, _CMP_NLT_UQ )}; }
		ND_ Bool4  LEqualU   (const Self &rhs)	const	{ return Bool4{ _mm256_cmp_pd( _value, rhs._value, _CMP_NGT_UQ )}; }

		
		ND_ Self  Floor ()						const	{ return Self{ _mm256_floor_pd( _value )}; }
		ND_ Self  Ceil ()						const	{ return Self{ _mm256_ceil_pd( _value )}; }
		template <int Mode> ND_ Self  Round ()	const	{ return Self{ _mm256_round_pd( _value, Mode )}; }

		ND_ SimdDouble2  Dot (const Self &rhs)			const
		{
			__m256d	xy		= _mm256_mul_pd( _value, rhs._value );
			__m256d	temp	= _mm256_hadd_pd( xy, xy );
			__m128d	hi128	= _mm256_extractf128_pd( temp, 1 );
			return SimdDouble2{ _mm_add_pd( _mm256_extractf128_pd( temp, 0 ), hi128 )};
		}

		ND_ static Self  DotV4 (const Self x[4], const Self y[4])
		{
			__m256d	xy0	= _mm256_mul_pd( x[0]._value, y[0]._value );
			__m256d	xy1	= _mm256_mul_pd( x[1]._value, y[1]._value );
			__m256d	xy2	= _mm256_mul_pd( x[2]._value, y[2]._value );
			__m256d	xy3	= _mm256_mul_pd( x[3]._value, y[3]._value );

			// low to high: xy00+xy01 xy10+xy11 xy02+xy03 xy12+xy13
			__m256d	temp01	= _mm256_hadd_pd( xy0, xy1 );   

			// low to high: xy20+xy21 xy30+xy31 xy22+xy23 xy32+xy33
			__m256d	temp23	= _mm256_hadd_pd( xy2, xy3 );

			// low to high: xy02+xy03 xy12+xy13 xy20+xy21 xy30+xy31
			__m256d	swapped	= _mm256_permute2f128_pd( temp01, temp23, 0x21 );

			// low to high: xy00+xy01 xy10+xy11 xy22+xy23 xy32+xy33
			__m256d	blended	= _mm256_blend_pd( temp01, temp23, 0b1100 );

			return Self{ _mm256_add_pd( swapped, blended )};
		}
		
		template <uint Idx> ND_ SimdDouble2  ToDouble2 () const { STATIC_ASSERT( Idx < 2 );  return SimdDouble2{ _mm256_extractf128_pd( _value, Idx )}; }

			void  ToArray (OUT Value_t* dst)	const	{ _mm256_storeu_pd( OUT dst, _value ); }
	};

# endif // AE_SIMD_AVX >= 1

# if AE_SIMD_AVX >= 2

	//
	// 256 bit int (AVX 2)
	//
	struct Int256b
	{
	// types
	public:
		using Self	= Int256b;


	// variables
	private:
		__m256i		_value;
		

	// methods
	public:
		Int256b () : _value{ _mm256_setzero_si256() } {}
		explicit Int256b (__m256i v) : _value{ v } {}
		
		ND_ Self  operator & (const Self &rhs)	const	{ return And( rhs ); }
		ND_ Self  operator | (const Self &rhs)	const	{ return Or( rhs ); }
		ND_ Self  operator ^ (const Self &rhs)	const	{ return Xor( rhs ); }
		
		ND_ Self  And (const Self &rhs)			const	{ return Self{ _mm256_and_si256( _value, rhs._value )}; }
		ND_ Self  Or  (const Self &rhs)			const	{ return Self{ _mm256_or_si256( _value, rhs._value )}; }
		ND_ Self  Xor (const Self &rhs)			const	{ return Self{ _mm256_xor_si256( _value, rhs._value )}; }
		ND_ Self  AndNot (const Self &rhs)		const	{ return Self{ _mm256_andnot_si256( _value, rhs._value )}; } // !a & b
		
		ND_ __m256i const&	Get ()				const	{ return _value; }

		// shift in bytes
		template <int ShiftBytes> ND_ Self  LShiftB ()	const	{ return Self{ _mm256_slli_si256( _value, ShiftBytes )}; }
		template <int ShiftBytes> ND_ Self  RShiftB ()	const	{ return Self{ _mm256_srli_si256( _value, ShiftBytes )}; }
	};



	//
	// 256 bit integer (AVX 2)
	//
	template <typename IntType>
	struct SimdTInt256
	{
		STATIC_ASSERT(( IsSameTypes<IntType, Int128b> or IsInteger<IntType> ));

	// types
	public:
		using Value_t	= IntType;
		using Self		= SimdTInt256< IntType >;
		using Bool256b	= SimdTInt256< IntType >;
		using Native_t	= __m256i;


	// variables
	private:
		Native_t	_value;		// int128[2], int64[4], int32[8], int16[16], int8[32]
		
		static constexpr bool	is8   = sizeof( IntType ) == 1;
		static constexpr bool	is16  = sizeof( IntType ) == 2;
		static constexpr bool	is32  = sizeof( IntType ) == 4;
		static constexpr bool	is64  = sizeof( IntType ) == 8;
		static constexpr bool	is128 = sizeof( IntType ) == 16;
		
		static constexpr bool	isU8  = IsSameTypes< IntType, ubyte >;
		static constexpr bool	isU16 = IsSameTypes< IntType, ushort >;
		static constexpr bool	isU32 = IsSameTypes< IntType, uint >;
		static constexpr bool	isU64 = IsSameTypes< IntType, ulong >;
		
		static constexpr bool	isI8  = IsSameTypes< IntType, sbyte >;
		static constexpr bool	isI16 = IsSameTypes< IntType, sshort >;
		static constexpr bool	isI32 = IsSameTypes< IntType, sint >;
		static constexpr bool	isI64 = IsSameTypes< IntType, slong >;
		
		static constexpr uint	count	= sizeof(_value) / sizeof(IntType);


	// methods
	public:
		SimdTInt256 () : _value{ _mm256_setzero_si256()} {}
		explicit SimdTInt256 (const Native_t &v) : _value{ v } {}
		
		explicit SimdTInt256 (IntType v)
		{
			if constexpr( isI8 )	_value = _mm256_set1_epi8( v );		else
			if constexpr( isU8 )	_value = _mm256_set1_epu8( v );		else
			if constexpr( is16 )	_value = _mm256_set1_epi16( v );	else
			if constexpr( is32 )	_value = _mm256_set1_epi32( v );	else
			if constexpr( is64 )	_value = _mm256_set1_epi64x( v );	else
			if constexpr( is128 )	_value = _mm256_set_m128( v, v );	else
									_value = v; // compilation error
		}
		
		explicit SimdTInt256 (Base::_hidden_::_UMax) :
			SimdTInt256{ ~IntType{0} }
		{}

		explicit SimdTInt256 (const IntType* v)
		{
			if constexpr( isI8 )	_value = _mm256_loadu_epi8( v );	else
			if constexpr( isU8 )	_value = _mm256_loadu_epu8( v );	else
			if constexpr( is16 )	_value = _mm256_loadu_epi16( v );	else
			if constexpr( is32 )	_value = _mm256_loadu_epi32( v );	else
			if constexpr( is64 )	_value = _mm256_loadu_epi64x( v );	else
									_value = v; // compilation error
		}

		template <typename = EnableIf<is8>>
		SimdTInt256 (IntType v00, IntType v01, IntType v02, IntType v03,
					 IntType v04, IntType v05, IntType v06, IntType v07,
					 IntType v08, IntType v09, IntType v10, IntType v11,
					 IntType v12, IntType v13, IntType v14, IntType v15,
					 IntType v16, IntType v17, IntType v18, IntType v19,
					 IntType v20, IntType v21, IntType v22, IntType v23,
					 IntType v24, IntType v25, IntType v26, IntType v27,
					 IntType v28, IntType v29, IntType v30, IntType v31) :
			_value{ _mm256_set_epi8( v00, v01, v02, v03, v04, v05, v06, v07,
									 v08, v09, v10, v11, v12, v13, v14, v15,
								     v16, v17, v18, v19, v20, v21, v22, v23,
									 v24, v25, v26, v27, v28, v29, v30, v31)} {}

		template <typename = EnableIf<is16>>
		SimdTInt256 (IntType v00, IntType v01, IntType v02, IntType v03,
					 IntType v04, IntType v05, IntType v06, IntType v07,
					 IntType v08, IntType v09, IntType v10, IntType v11,
					 IntType v12, IntType v13, IntType v14, IntType v15) :
			_value{ _mm256_set_epi16( v00, v01, v02, v03, v04, v05, v06, v07,
									  v08, v09, v10, v11, v12, v13, v14, v15 )} {}

		template <typename = EnableIf<is32>>
		SimdTInt256 (IntType v0, IntType v1, IntType v2, IntType v3, IntType v4, IntType v5, IntType v6, IntType v7) :
			_value{ _mm256_set_epi32( v0, v1, v2, v3, v4, v5, v6, v7 )} {}
		
		template <typename = EnableIf<is64>>
		SimdTInt256 (IntType v0, IntType v1, IntType v2, IntType v3) :
			_value{ _mm256_set_epi64x( v0, v1, v2, v3 )} {}
		
		template <typename = EnableIf<is128>>
		SimdTInt256 (IntType v0, IntType v1) :
			_value{ _mm256_set_m128( v0, v1 )} {}
		
		ND_ Self  operator - ()						const	{ return Negative(); }
		ND_ Self  operator ~ ()						const	{ return Not(); }

		ND_ Self  operator + (const Self &rhs)		const	{ return Add( rhs ); }
		ND_ Self  operator - (const Self &rhs)		const	{ return Sub( rhs ); }
		ND_ Self  operator * (const Self &rhs)		const	{ return Mul( rhs ); }
		ND_ Self  operator / (const Self &rhs)		const	{ return Div( rhs ); }
		
		ND_ Self  operator & (const Self &rhs)		const	{ return And( rhs ); }
		ND_ Self  operator | (const Self &rhs)		const	{ return Or( rhs ); }
		ND_ Self  operator ^ (const Self &rhs)		const	{ return Xor( rhs ); }
		
		ND_ Self  operator << (int shift)			const	{ return LShift( shift ); }
		ND_ Self  operator << (const Self &rhs)		const	{ return LShift( rhs ); }
		ND_ Self  operator >> (int shift)			const	{ return RShift( shift ); }
		ND_ Self  operator >> (const Self &rhs)		const	{ return RShift( rhs ); }

		ND_ Bool256b  operator == (const Self &rhs)	const	{ return Equal( rhs ); }
		ND_ Bool256b  operator != (const Self &rhs)	const	{ return NotEqual( rhs ); }
		ND_ Bool256b  operator >  (const Self &rhs)	const	{ return Greater( rhs ); }
		ND_ Bool256b  operator <  (const Self &rhs)	const	{ return Less( rhs ); }
		ND_ Bool256b  operator >= (const Self &rhs)	const	{ return GEqual( rhs ); }
		ND_ Bool256b  operator <= (const Self &rhs)	const	{ return LEqual( rhs ); }
		
		
		ND_ IntType  operator [] (usize i)	const
		{
			ASSERT( i < count );
			if constexpr( isI8 )	return _value.m256i_i8[i];
			if constexpr( isU8 )	return _value.m256i_u8[i];
			if constexpr( isI16 )	return _value.m256i_i16[i];
			if constexpr( isU16 )	return _value.m256i_u16[i];
			if constexpr( isI32 )	return _value.m256i_i32[i];
			if constexpr( isU32 )	return _value.m256i_u32[i];
			if constexpr( isI64 )	return _value.m256i_i64[i];
			if constexpr( isU64 )	return _value.m256i_u64[i];
			if constexpr( is128 )	return IntType{ _mm256_extracti128_si256( _value, i )};
		}
		
		ND_ Native_t const&  Get ()	const	{ return _value; }
		
		ND_ Self	Negative ()		const	{ return Self{0}.Sub( *this ); }		// TODO: optimize ?

		ND_ Self  Add (const Self &rhs)	const
		{
			if constexpr( is8 )		return Self{ _mm256_add_epi8( _value, rhs._value )};
			if constexpr( is16 )	return Self{ _mm256_add_epi16( _value, rhs._value )};
			if constexpr( is32 )	return Self{ _mm256_add_epi32( _value, rhs._value )};
			if constexpr( is64 )	return Self{ _mm256_add_epi64( _value, rhs._value )};
		}
		
		ND_ Self  AddSaturate (const Self &rhs)	const
		{
			if constexpr( isI8 )	return Self{ _mm256_adds_epi8(  _value, rhs._value )};
			if constexpr( isU8 )	return Self{ _mm256_adds_epu8(  _value, rhs._value )};
			if constexpr( isI16 )	return Self{ _mm256_adds_epi16( _value, rhs._value )};
			if constexpr( isU16 )	return Self{ _mm256_adds_epu16( _value, rhs._value )};
		}

		ND_ Self  Sub (const Self &rhs)	const
		{
			if constexpr( is8 )		return Self{ _mm256_sub_epi8( _value, rhs._value )};
			if constexpr( is16 )	return Self{ _mm256_sub_epi16( _value, rhs._value )};
			if constexpr( is32 )	return Self{ _mm256_sub_epi32( _value, rhs._value )};
			if constexpr( is64 )	return Self{ _mm256_sub_epi64( _value, rhs._value )};
		}
		
		ND_ Self  SubSaturate (const Self &rhs)	const
		{
			if constexpr( isI8 )	return Self{ _mm256_subs_epi8(  _value, rhs._value )};
			if constexpr( isU8 )	return Self{ _mm256_subs_epu8(  _value, rhs._value )};
			if constexpr( isI16 )	return Self{ _mm256_subs_epi16( _value, rhs._value )};
			if constexpr( isU16 )	return Self{ _mm256_subs_epu16( _value, rhs._value )};
		}

		ND_ Self  Mul (const Self &rhs)		const
		{
			if constexpr( isI32 )	return Self{ _mm256_mul_epi32( _value, rhs._value )};
			if constexpr( isU32 )	return Self{ _mm256_mul_epu32( _value, rhs._value )};
		}
		
		
		ND_ Self	Not ()						const	{ return AndNot( Self{UMax} ); }
		ND_ Self	And (const Self &rhs)		const	{ return Self{ _mm256_and_si256( _value, rhs._value )}; }
		ND_ Self	Or  (const Self &rhs)		const	{ return Self{ _mm256_or_si256( _value, rhs._value )}; }
		ND_ Self	Xor (const Self &rhs)		const	{ return Self{ _mm256_xor_si256( _value, rhs._value )}; }
		ND_ Self	AndNot (const Self &rhs)	const	{ return Self{ _mm256_andnot_si256( _value, rhs._value )}; } // ~a & b
		
		
		ND_ Self  Min (const Self &rhs)	const
		{
			if constexpr( isI8 )	return Self{ _mm256_min_epi8(  _value, rhs._value )};
			if constexpr( isU8 )	return Self{ _mm256_min_epu8(  _value, rhs._value )};
			if constexpr( isI16 )	return Self{ _mm256_min_epi16( _value, rhs._value )};
			if constexpr( isU16 )	return Self{ _mm256_min_epu16( _value, rhs._value )};
			if constexpr( isI32 )	return Self{ _mm256_min_epi32( _value, rhs._value )};
			if constexpr( isU32 )	return Self{ _mm256_min_epu32( _value, rhs._value )};
			if constexpr( isI64 )	return Self{ _mm256_min_epi64( _value, rhs._value )};
			if constexpr( isU64 )	return Self{ _mm256_min_epu64( _value, rhs._value )};
		}
		
		ND_ Self  Max (const Self &rhs)	const
		{
			if constexpr( isI8 )	return Self{ _mm256_max_epi8(  _value, rhs._value )};
			if constexpr( isU8 )	return Self{ _mm256_max_epu8(  _value, rhs._value )};
			if constexpr( isI16 )	return Self{ _mm256_max_epi16( _value, rhs._value )};
			if constexpr( isU16 )	return Self{ _mm256_max_epu16( _value, rhs._value )};
			if constexpr( isI32 )	return Self{ _mm256_max_epi32( _value, rhs._value )};
			if constexpr( isU32 )	return Self{ _mm256_max_epu32( _value, rhs._value )};
			if constexpr( isI64 )	return Self{ _mm256_max_epi64( _value, rhs._value )};
			if constexpr( isU64 )	return Self{ _mm256_max_epu64( _value, rhs._value )};
		}

		ND_ Bool256b  Equal (const Self &rhs) const
		{
			if constexpr( is8 )		return Self{ _mm256_cmpeq_epi8( _value, rhs._value )};
			if constexpr( is16 )	return Self{ _mm256_cmpeq_epi16( _value, rhs._value )};
			if constexpr( is32 )	return Self{ _mm256_cmpeq_epi32( _value, rhs._value )};
			if constexpr( is64 )	return Self{ _mm256_cmpeq_epi64( _value, rhs._value )};
		}

		ND_ Bool256b  Greater (const Self &rhs) const
		{
			if constexpr( is8 )		return Self{ _mm256_cmpgt_epi8( _value, rhs._value )};
			if constexpr( is16 )	return Self{ _mm256_cmpgt_epi16( _value, rhs._value )};
			if constexpr( is32 )	return Self{ _mm256_cmpgt_epi32( _value, rhs._value )};
			if constexpr( is64 )	return Self{ _mm256_cmpgt_epi64( _value, rhs._value )};
		}

		ND_ Bool256b  Less (const Self &rhs) const
		{
			if constexpr( is8 )		return Self{ _mm256_cmplt_epi8( _value, rhs._value )};
			if constexpr( is16 )	return Self{ _mm256_cmplt_epi16( _value, rhs._value )};
			if constexpr( is32 )	return Self{ _mm256_cmplt_epi32( _value, rhs._value )};
			if constexpr( is64 )	return Self{ _mm256_cmplt_epi64( _value, rhs._value )};
		}


		// zero on overflow
		ND_ Self  LShift (int shift)	const
		{
			if constexpr( is16 )	return Self{ _mm256_slli_epi16( _value, shift )};
			if constexpr( is32 )	return Self{ _mm256_slli_epi32( _value, shift )};
			if constexpr( is64 )	return Self{ _mm256_slli_epi64( _value, shift )};
		}
		
		ND_ Self  LShift (const Self &rhs)	const
		{
			if constexpr( is16 )	return Self{ _mm256_sll_epi16( _value, rhs._value )};
			if constexpr( is32 )	return Self{ _mm256_sll_epi32( _value, rhs._value )};
			if constexpr( is64 )	return Self{ _mm256_sll_epi64( _value, rhs._value )};
		}

		
		// zero on overflow
		ND_ Self  RShift (int shift)	const
		{
			if constexpr( is16 )	return Self{ _mm256_srli_epi16( _value, shift )};
			if constexpr( is32 )	return Self{ _mm256_srli_epi32( _value, shift )};
			if constexpr( is64 )	return Self{ _mm256_srli_epi64( _value, shift )};
		}
		
		ND_ Self  RShift (const Self &rhs)	const
		{
			if constexpr( is16 )	return Self{ _mm256_srl_epi16( _value, rhs._value )};
			if constexpr( is32 )	return Self{ _mm256_srl_epi32( _value, rhs._value )};
			if constexpr( is64 )	return Self{ _mm256_srl_epi64( _value, rhs._value )};
		}

		
		// max on overflow
		ND_ Self  RShiftA (int shift)	const
		{
			if constexpr( is16 )	return Self{ _mm256_srai_epi16( _value, shift )};
			if constexpr( is32 )	return Self{ _mm256_srai_epi32( _value, shift )};
			if constexpr( is64 )	return Self{ _mm256_srai_epi64( _value, shift )};
		}
		
		ND_ Self  RShiftA (const Self &rhs)	const
		{
			if constexpr( is16 )	return Self{ _mm256_sra_epi16( _value, rhs._value )};
			if constexpr( is32 )	return Self{ _mm256_sra_epi32( _value, rhs._value )};
			if constexpr( is64 )	return Self{ _mm256_sra_epi64( _value, rhs._value )};
		}

		
		// non SIMD
		ND_ bool  All () const
		{
			if constexpr( is8 )		{ IntType accum = UMax;  for (uint i = 0; i < count; ++i) accum &= _value.m256i_u8[i];   return accum == UMax; }
			if constexpr( is16 )	{ IntType accum = UMax;  for (uint i = 0; i < count; ++i) accum &= _value.m256i_u16[i];  return accum == UMax; }
			if constexpr( is32 )	{ IntType accum = UMax;  for (uint i = 0; i < count; ++i) accum &= _value.m256i_u32[i];  return accum == UMax; }
			if constexpr( is64 )	return (_value.m256i_u64[0] & _value.m256i_u64[1] & _value.m256i_u64[2] & _value.m256i_u64[3]) == UMax;
		}
		
		ND_ bool  Any () const
		{
			if constexpr( is8 )		{ IntType accum = 0;  for (uint i = 0; i < count; ++i) accum |= _value.m256i_u8[i];   return accum == UMax; }
			if constexpr( is16 )	{ IntType accum = 0;  for (uint i = 0; i < count; ++i) accum |= _value.m256i_u16[i];  return accum == UMax; }
			if constexpr( is32 )	{ IntType accum = 0;  for (uint i = 0; i < count; ++i) accum |= _value.m256i_u32[i];  return accum == UMax; }
			if constexpr( is64 )	return (_value.m256i_u64[0] | _value.m256i_u64[1] | _value.m256i_u64[2] | _value.m256i_u64[3]) == UMax;
		}
		
		ND_ bool  None () const
		{
			if constexpr( is8 )		{ IntType accum = 0;  for (uint i = 0; i < count; ++i) accum |= _value.m256i_u8[i];   return accum == 0; }
			if constexpr( is16 )	{ IntType accum = 0;  for (uint i = 0; i < count; ++i) accum |= _value.m256i_u16[i];  return accum == 0; }
			if constexpr( is32 )	{ IntType accum = 0;  for (uint i = 0; i < count; ++i) accum |= _value.m256i_u32[i];  return accum == 0; }
			if constexpr( is64 )	return (_value.m256i_u64[0] | _value.m256i_u64[1] | _value.m256i_u64[2] | _value.m256i_u64[3]) == 0;
		}

		void  ToArray (OUT IntType* dst)	const
		{
			if constexpr( is8 )		_mm256_storeu_epi8(  OUT static_cast<void*>(dst), _value );
			if constexpr( is16 )	_mm256_storeu_epi16( OUT static_cast<void*>(dst), _value );
			if constexpr( is32 )	_mm256_storeu_epi32( OUT static_cast<void*>(dst), _value );
			if constexpr( is64 )	_mm256_storeu_epi64( OUT static_cast<void*>(dst), _value );
		}

		// TODO: AES

		template <typename IT>
		ND_ SimdTInt256<IT> &		Cast ()			{ return reinterpret_cast< SimdTInt256<IT> &>(*this); }

		template <typename IT>
		ND_ SimdTInt256<IT> const&	Cast ()	const	{ return reinterpret_cast< SimdTInt256<IT> const &>(*this); }
		
		ND_ Int256b const&			Cast ()	const	{ return reinterpret_cast< Int256b const &>(*this); }
	};

#  endif // AE_SIMD_AVX >= 2

	/*
# if AE_SIMD_AVX >= 3
	//
	// 128 bit half (AVX512_FP16 + AVX512VL)
	//
	struct SimdHalf8
	{
	// types
	public:
		using Value_t	= glm::detail::hdata;
		using Self		= SimdHalf8;

		
	// variables
	private:
		__m128		_value;		// half[8]
		

	// methods
	public:
		SimdHalf8 () : _value{_mm_setzero_ps()}	{}
	};



	//
	// 256 bit half (AVX512_FP16 + AVX512VL)
	//
	struct SimdHalf16
	{
	// types
	public:
		using Value_t	= glm::detail::hdata;
		using Self		= SimdHalf16;

		
	// variables
	private:
		__m256		_value;		// half[16]
		

	// methods
	public:
		SimdHalf16 () : _value{_mm256_setzero_ps()}	{}
	};
# endif // AE_SIMD_AVX >= 3
	*/

//-----------------------------------------------------------------------------


# if AE_SIMD_AVX >= 3

	//
	// 512 bit float (AVX512)
	//
	struct SimdFloat16
	{
	// types
	public:
		using Value_t	= float;
		using Self		= SimdFloat16;


	// variables
	private:
		__m512		_value;		// float[16]


	// methods
	public:
		SimdFloat16 () : _value{_mm512_setzero_ps()}	{}
	};
	


	//
	// 512 bit double (AVX512)
	//
	struct SimdDouble8
	{
	// types
	public:
		using Value_t	= double;
		using Self		= SimdDouble8;


	// variables
	private:
		__m512d		_value;		// doubel[8]


	// methods
	public:
		SimdDouble8 () : _value{_mm512_setzero_pd()}	{}
	};
	
	

	//
	// 512 bit integer (AVX512)
	//
	struct Int512b
	{
	// types
	public:
		using Self	= Int512b;

		
	// variables
	private:
		__m512i		_value;


	// methods
	public:
		Int512b () : _value{ _mm512_setzero_si512() } {}
	};



	//
	// 512 bit integer (AVX512)
	//
	template <typename IntType>
	struct SimdTInt512
	{
		STATIC_ASSERT(( IsSameTypes<IntType, Int128b> or IsSameTypes<IntType, Int256b> or IsInteger<IntType> ));

	// types
	public:
		using Value_t	= IntType;
		using Self		= SimdTInt512< IntType >;


	// variables
	private:
		__m512i		_value;		// long[8], int[16], short[32], byte[64]


	// methods
	public:
		SimdTInt512 () : _value{_mm512_setzero_si512() } {}

		template <typename IT>
		ND_ SimdTInt512<IT> &		Cast ()			{ return reinterpret_cast< SimdTInt512<IT> &>(*this); }

		template <typename IT>
		ND_ SimdTInt512<IT> const&	Cast ()	const	{ return reinterpret_cast< SimdTInt512<IT> const &>(*this); }

		ND_ Int512b const&			Cast ()	const	{ return reinterpret_cast< Int512b const &>(*this); }
	};

# endif // AE_SIMD_AVX >= 3
//-----------------------------------------------------------------------------


} // AE::Math
