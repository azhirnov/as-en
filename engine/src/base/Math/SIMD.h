// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	ref:
	https://software.intel.com/sites/landingpage/IntrinsicsGuide
	ps    -- packed single precision
	pd    -- packed double precision
	epi32 -- packed 32-bit integers
	epu32 -- packed 32-bit unsigned integers
	epi64 -- packed 64-bit integers
*/

#pragma once

#include "base/Common.h"

#if defined(AE_CPU_ARCH_X64) or defined(AE_CPU_ARCH_X86)
	// AVX
#	define AE_SIMD_AVX
#	include <immintrin.h>
	// AVX 512
#	define AE_SIMD_AVX512
//#	define AE_SIMD_AVX512_FP16
//#	include <zmmintrin.h>	// included in 'immintrin.h'
	// SSE 4.2
#	define AE_SIMD_SSE
#	include <nmmintrin.h>
	// AES
#	define AE_SIMD_AES
#	include <wmmintrin.h>
#endif

#if (defined(AE_CPU_ARCH_ARM32) or defined(AE_CPU_ARCH_ARM64)) and defined(__ARM_NEON__)
#	define AE_SIMD_NEON
#	include <arm_neon.h>
	// TODO: arm64_neon.h
#endif

#include "base/Math/GLM.h"


namespace AE::Math
{
	struct Int128b;
	struct SimdFloat4;
	struct SimdDouble2;
	template <typename IntType> struct SimdTInt128;
	using SimdByte16	= SimdTInt128< sbyte >;
	using SimdUByte16	= SimdTInt128< ubyte >;
	using SimdShort8	= SimdTInt128< short >;
	using SimdUShort8	= SimdTInt128< ushort >;
	using SimdInt4		= SimdTInt128< int >;
	using SimdUInt4		= SimdTInt128< uint >;
	using SimdLong2		= SimdTInt128< slong >;
	using SimdULong2	= SimdTInt128< ulong >;

	struct Int256b;
	struct SimdFloat8;
	struct SimdDouble4;
	template <typename IntType>	struct SimdTInt256;
	using SimdByte32	= SimdTInt256< sbyte >;
	using SimdUByte32	= SimdTInt256< ubyte >;
	using SimdShort16	= SimdTInt256< short >;
	using SimdUShort16	= SimdTInt256< ushort >;
	using SimdInt8		= SimdTInt256< int >;
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
	using SimdShort32	= SimdTInt512< short >;
	using SimdUShort32	= SimdTInt512< ushort >;
	using SimdInt16		= SimdTInt512< int >;
	using SimdUInt16	= SimdTInt512< uint >;
	using SimdLong8		= SimdTInt512< slong >;
	using SimdULong8	= SimdTInt512< ulong >;
	using SimdInt128b4	= SimdTInt512< Int128b >;
	using SimdInt256b2	= SimdTInt512< Int256b >;


# ifdef AE_SIMD_SSE
	
	//
	// 128 bit float (SSE 2.x)
	//
	struct SimdFloat4
	{
	// types
	public:
		using value_type	= float;
		using Self			= SimdFloat4;


	// variables
	private:
		__m128	_value;		// float[4]


	// methods
	public:
		SimdFloat4 () : _value{ _mm_setzero_ps() } {}
		explicit SimdFloat4 (float v) : _value{ _mm_set1_ps( v )} {}
		explicit SimdFloat4 (const float *v) : _value{ _mm_loadu_ps( v )} {}
		explicit SimdFloat4 (const __m128 &v) : _value{ v } {}
		SimdFloat4 (float x, float y, float z, float w) : _value{ _mm_set_ps( x, y, z, w )} {}

		ND_ SimdFloat4	operator +  (const SimdFloat4 &rhs)	const	{ return Add( rhs ); }
		ND_ SimdFloat4	operator -  (const SimdFloat4 &rhs)	const	{ return Sub( rhs ); }
		ND_ SimdFloat4	operator *  (const SimdFloat4 &rhs)	const	{ return Mul( rhs ); }
		ND_ SimdFloat4	operator /  (const SimdFloat4 &rhs)	const	{ return Div( rhs ); }
		
		ND_ SimdFloat4	operator &  (const SimdFloat4 &rhs)	const	{ return And( rhs ); }
		ND_ SimdFloat4	operator |  (const SimdFloat4 &rhs)	const	{ return Or( rhs ); }
		ND_ SimdFloat4	operator ^  (const SimdFloat4 &rhs)	const	{ return Xor( rhs ); }

		ND_ SimdFloat4	operator == (const SimdFloat4 &rhs)	const	{ return Equal( rhs ); }
		ND_ SimdFloat4	operator != (const SimdFloat4 &rhs)	const	{ return NotEqual( rhs ); }
		ND_ SimdFloat4	operator >  (const SimdFloat4 &rhs)	const	{ return Greater( rhs ); }
		ND_ SimdFloat4	operator <  (const SimdFloat4 &rhs)	const	{ return Less( rhs ); }
		ND_ SimdFloat4	operator >= (const SimdFloat4 &rhs)	const	{ return GEqual( rhs ); }
		ND_ SimdFloat4	operator <= (const SimdFloat4 &rhs)	const	{ return LEqual( rhs ); }

		ND_ float		operator [] (usize i)				const	{ ASSERT( i < 4 );  return _value.m128_f32[i]; }


		ND_ SimdFloat4	Add (const SimdFloat4 &rhs)			const	{ return SimdFloat4{ _mm_add_ps( _value, rhs._value )}; }
		ND_ SimdFloat4	Sub (const SimdFloat4 &rhs)			const	{ return SimdFloat4{ _mm_sub_ps( _value, rhs._value )}; }
		ND_ SimdFloat4	Mul (const SimdFloat4 &rhs)			const	{ return SimdFloat4{ _mm_mul_ps( _value, rhs._value )}; }
		ND_ SimdFloat4	Div (const SimdFloat4 &rhs)			const	{ return SimdFloat4{ _mm_div_ps( _value, rhs._value )}; }
		ND_ SimdFloat4	Min (const SimdFloat4 &rhs)			const	{ return SimdFloat4{ _mm_min_ps( _value, rhs._value )}; }
		ND_ SimdFloat4	Max (const SimdFloat4 &rhs)			const	{ return SimdFloat4{ _mm_max_ps( _value, rhs._value )}; }
		
		ND_ SimdFloat4	And (const SimdFloat4 &rhs)			const	{ return SimdFloat4{ _mm_and_ps( _value, rhs._value )}; }
		ND_ SimdFloat4	Or  (const SimdFloat4 &rhs)			const	{ return SimdFloat4{ _mm_or_ps( _value, rhs._value )}; }
		ND_ SimdFloat4	Xor (const SimdFloat4 &rhs)			const	{ return SimdFloat4{ _mm_xor_ps( _value, rhs._value )}; }
		ND_ SimdFloat4	AndNot (const SimdFloat4 &rhs)		const	{ return SimdFloat4{ _mm_andnot_ps( _value, rhs._value )}; }	// !a & b

		// returns { a[0] + a[1],  a[2] + a[3],  b[0] + b[1],  b[2] + b[3] }
		ND_ SimdFloat4	HAdd (const SimdFloat4 &rhs)		const	{ return SimdFloat4{ _mm_hadd_ps( _value, rhs._value )}; }		// SSE 3
		
		// returns { a[0] - b[0], a[1] + b[1], a[2] - b[2], a[3] + b[3] }
		ND_ SimdFloat4	AddSub (const SimdFloat4 &rhs)		const	{ return SimdFloat4{ _mm_addsub_ps( _value, rhs._value )}; }	// SSE 3

		ND_ SimdFloat4	Sqrt ()								const	{ return SimdFloat4{ _mm_sqrt_ps( _value )}; }
		ND_ SimdFloat4	Reciporal ()						const	{ return SimdFloat4{ _mm_rcp_ps( _value )}; }		// 1 / x
		ND_ SimdFloat4	RSqrt ()							const	{ return SimdFloat4{ _mm_rsqrt_ps( _value )}; }		// 1 / sqrt(x)

		// returns 0 or ~0u
		ND_ SimdFloat4	Equal    (const SimdFloat4 &rhs)	const	{ return SimdFloat4{ _mm_cmpeq_ps( _value, rhs._value )}; }
		ND_ SimdFloat4	NotEqual (const SimdFloat4 &rhs)	const	{ return SimdFloat4{ _mm_cmpneq_ps( _value, rhs._value )}; }
		ND_ SimdFloat4	Greater  (const SimdFloat4 &rhs)	const	{ return SimdFloat4{ _mm_cmpgt_ps( _value, rhs._value )}; }
		ND_ SimdFloat4	Less     (const SimdFloat4 &rhs)	const	{ return SimdFloat4{ _mm_cmplt_ps( _value, rhs._value )}; }
		ND_ SimdFloat4	GEqual   (const SimdFloat4 &rhs)	const	{ return SimdFloat4{ _mm_cmpge_ps( _value, rhs._value )}; }
		ND_ SimdFloat4	LEqual   (const SimdFloat4 &rhs)	const	{ return SimdFloat4{ _mm_cmple_ps( _value, rhs._value )}; }

		// TODO: _mm_cmp_ps AVX
		
		ND_ SimdFloat4	DotV (const SimdFloat4 &rhs)		const	{ return SimdFloat4{ _mm_dp_ps( _value, rhs._value, 0xFF )}; }		// SSE 4.1
		ND_ float		Dot  (const SimdFloat4 &rhs)		const	{ return _mm_cvtss_f32( _mm_dp_ps( _value, rhs._value, 0xFF )); }	// SSE 4.1

			void ToArray (OUT float* dst)					const	{ _mm_storeu_ps( OUT dst, _value ); }

		template <typename IT>
		ND_ EnableIf< IsInteger<IT>, SimdTInt128<IT>>	Cast () const;

		template <typename T>
		ND_ EnableIf< IsSameTypes<T, double>, SimdDouble2>	Cast () const;
	};
	


	//
	// 128 bit double (SSE 2.x)
	//
	struct SimdDouble2
	{
	// types
	public:
		using value_type	= double;
		using Self			= SimdDouble2;


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

		ND_ SimdDouble2	operator +  (const SimdDouble2 &rhs)	const	{ return Add( rhs ); }
		ND_ SimdDouble2	operator -  (const SimdDouble2 &rhs)	const	{ return Sub( rhs ); }
		ND_ SimdDouble2	operator *  (const SimdDouble2 &rhs)	const	{ return Mul( rhs ); }
		ND_ SimdDouble2	operator /  (const SimdDouble2 &rhs)	const	{ return Div( rhs ); }
		
		ND_ SimdDouble2	operator &  (const SimdDouble2 &rhs)	const	{ return And( rhs ); }
		ND_ SimdDouble2	operator |  (const SimdDouble2 &rhs)	const	{ return Or( rhs ); }
		ND_ SimdDouble2	operator ^  (const SimdDouble2 &rhs)	const	{ return Xor( rhs ); }

		ND_ SimdDouble2	operator == (const SimdDouble2 &rhs)	const	{ return Equal( rhs ); }
		ND_ SimdDouble2	operator != (const SimdDouble2 &rhs)	const	{ return NotEqual( rhs ); }
		ND_ SimdDouble2	operator >  (const SimdDouble2 &rhs)	const	{ return Greater( rhs ); }
		ND_ SimdDouble2	operator <  (const SimdDouble2 &rhs)	const	{ return Less( rhs ); }
		ND_ SimdDouble2	operator >= (const SimdDouble2 &rhs)	const	{ return GEqual( rhs ); }
		ND_ SimdDouble2	operator <= (const SimdDouble2 &rhs)	const	{ return LEqual( rhs ); }
		
		ND_ double		operator [] (usize i)					const	{ ASSERT( i < 2 );  return _value.m128d_f64[i]; }


		ND_ SimdDouble2	Add (const SimdDouble2 &rhs)			const	{ return SimdDouble2{ _mm_add_pd( _value, rhs._value )}; }
		ND_ SimdDouble2	Sub (const SimdDouble2 &rhs)			const	{ return SimdDouble2{ _mm_sub_pd( _value, rhs._value )}; }
		ND_ SimdDouble2	Mul (const SimdDouble2 &rhs)			const	{ return SimdDouble2{ _mm_mul_pd( _value, rhs._value )}; }
		ND_ SimdDouble2	Div (const SimdDouble2 &rhs)			const	{ return SimdDouble2{ _mm_div_pd( _value, rhs._value )}; }
		ND_ SimdDouble2	Min (const SimdDouble2 &rhs)			const	{ return SimdDouble2{ _mm_min_pd( _value, rhs._value )}; }
		ND_ SimdDouble2	Max (const SimdDouble2 &rhs)			const	{ return SimdDouble2{ _mm_max_pd( _value, rhs._value )}; }
		
		ND_ SimdDouble2	And (const SimdDouble2 &rhs)			const	{ return SimdDouble2{ _mm_and_pd( _value, rhs._value )}; }
		ND_ SimdDouble2	Or  (const SimdDouble2 &rhs)			const	{ return SimdDouble2{ _mm_or_pd( _value, rhs._value )}; }
		ND_ SimdDouble2	Xor (const SimdDouble2 &rhs)			const	{ return SimdDouble2{ _mm_xor_pd( _value, rhs._value )}; }
		ND_ SimdDouble2	AndNot (const SimdDouble2 &rhs)			const	{ return SimdDouble2{ _mm_andnot_pd( _value, rhs._value )}; }	// !a & b

		// returns { a[0] + a[1],  b[0] + b[1] }
		ND_ SimdDouble2	HAdd (const SimdDouble2 &rhs)			const	{ return SimdDouble2{ _mm_hadd_pd( _value, rhs._value )}; }		// SSE 3
		
		// returns { a[0] - b[0], a[1] + b[1] }
		ND_ SimdDouble2	AddSub (const SimdDouble2 &rhs)			const	{ return SimdDouble2{ _mm_addsub_pd( _value, rhs._value )}; }	// SSE 3

		// returns 0 or ~0ull
		ND_ SimdDouble2	Equal    (const SimdDouble2 &rhs)		const	{ return SimdDouble2{ _mm_cmpeq_pd( _value, rhs._value )}; }
		ND_ SimdDouble2	NotEqual (const SimdDouble2 &rhs)		const	{ return SimdDouble2{ _mm_cmpneq_pd( _value, rhs._value )}; }
		ND_ SimdDouble2	Greater  (const SimdDouble2 &rhs)		const	{ return SimdDouble2{ _mm_cmpgt_pd( _value, rhs._value )}; }
		ND_ SimdDouble2	Less     (const SimdDouble2 &rhs)		const	{ return SimdDouble2{ _mm_cmplt_pd( _value, rhs._value )}; }
		ND_ SimdDouble2	GEqual   (const SimdDouble2 &rhs)		const	{ return SimdDouble2{ _mm_cmpge_pd( _value, rhs._value )}; }
		ND_ SimdDouble2	LEqual   (const SimdDouble2 &rhs)		const	{ return SimdDouble2{ _mm_cmple_pd( _value, rhs._value )}; }
		
		ND_ double		Dot  (const SimdDouble2 &rhs)			const	{ return DotV( rhs )[0]; }
		ND_ SimdDouble2	DotV (const SimdDouble2 &rhs)			const
		{
			__m128d	ab = _mm_mul_pd( _value, rhs._value );
			return SimdDouble2{ _mm_add_pd( ab, _mm_shuffle_pd( ab, ab, _MM_SHUFFLE2(1, 0) ))};
		}

		// TODO: _mm_cmp_pd AVX

		ND_ SimdDouble2	Sqrt ()									const	{ return SimdDouble2{ _mm_sqrt_pd( _value )}; }

			void ToArray (OUT double* dst)						const	{ _mm_storeu_pd( OUT dst, _value ); }
		
		template <typename IT>
		ND_ EnableIf< IsInteger<IT>, SimdTInt128<IT>>	Cast () const;
	};



	//
	// 128 bit int
	//
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
		ND_ Self	AndNot (const Self &rhs)		const	{ return Self{ _mm_andnot_si128( _value, rhs._value )}; }	// !a & b
		
		// shift in bytes
		template <int ShiftBytes>	ND_ Self	LShiftB ()	const	{ return Self{ _mm_bslli_si128( _value, ShiftBytes )}; }
		template <int ShiftBytes>	ND_ Self	RShiftB ()	const	{ return Self{ _mm_bsrli_si128( _value, ShiftBytes )}; }

		ND_ int		ToInt32 ()						const	{ return _mm_cvtsi128_si32( _value ); }
		ND_ slong	ToInt64 ()						const	{ return _mm_cvtsi128_si64( _value ); }
	};



	//
	// 128 bit integer (SSE 2.x)
	//
	template <typename IntType>
	struct SimdTInt128
	{
		STATIC_ASSERT( IsInteger<IntType> );

	// types
	public:
		using value_type	= IntType;
		using Self			= SimdTInt128< IntType >;


	// variables
	private:
		__m128i		_value;		// int64[2], int32[4], int16[8], int8[16]

		static constexpr bool	is8  = sizeof( IntType ) == 1;
		static constexpr bool	is16 = sizeof( IntType ) == 2;
		static constexpr bool	is32 = sizeof( IntType ) == 4;
		static constexpr bool	is64 = sizeof( IntType ) == 8;
		
		static constexpr bool	isU8  = IsSameTypes< IntType, sbyte >;
		static constexpr bool	isU16 = IsSameTypes< IntType, short >;
		static constexpr bool	isU32 = IsSameTypes< IntType, int >;
		static constexpr bool	isU64 = IsSameTypes< IntType, slong >;
		
		static constexpr bool	isI8  = IsSameTypes< IntType, ubyte >;
		static constexpr bool	isI16 = IsSameTypes< IntType, ushort >;
		static constexpr bool	isI32 = IsSameTypes< IntType, uint >;
		static constexpr bool	isI64 = IsSameTypes< IntType, ulong >;


	// methods
	public:
		SimdTInt128 () : _value{ _mm_setzero_si128() } {}
		explicit SimdTInt128 (const __m128i &v) : _value{ v } {}
		
		explicit SimdTInt128 (IntType v)
		{
			if constexpr( is8 )		_value = _mm_set1_epi8( v );	else
			if constexpr( is16 )	_value = _mm_set1_epi16( v );	else
			if constexpr( is32 )	_value = _mm_set1_epi32( v );	else
			if constexpr( is64 )	_value = _mm_set1_epi64x( v );
		}

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

		ND_ Self	operator == (const Self &rhs)	const	{ return Equal( rhs ); }
		ND_ Self	operator != (const Self &rhs)	const	{ return NotEqual( rhs ); }
		ND_ Self	operator >  (const Self &rhs)	const	{ return Greater( rhs ); }
		ND_ Self	operator <  (const Self &rhs)	const	{ return Less( rhs ); }
		ND_ Self	operator >= (const Self &rhs)	const	{ return GEqual( rhs ); }
		ND_ Self	operator <= (const Self &rhs)	const	{ return LEqual( rhs ); }
		

		ND_ EnableIf<isI8,  IntType>	operator [] (usize i)	const	{ ASSERT( i < 16 ); return _value.m128i_i8[i]; }
		ND_ EnableIf<isU8,  IntType>	operator [] (usize i)	const	{ ASSERT( i < 16 ); return _value.m128i_u8[i]; }
		ND_ EnableIf<isI16, IntType>	operator [] (usize i)	const	{ ASSERT( i < 8 );  return _value.m128i_i16[i]; }
		ND_ EnableIf<isU16, IntType>	operator [] (usize i)	const	{ ASSERT( i < 8 );  return _value.m128i_u16[i]; }
		ND_ EnableIf<isI32, IntType>	operator [] (usize i)	const	{ ASSERT( i < 4 );  return _value.m128i_i32[i]; }
		ND_ EnableIf<isU32, IntType>	operator [] (usize i)	const	{ ASSERT( i < 4 );  return _value.m128i_u32[i]; }
		ND_ EnableIf<isI64, IntType>	operator [] (usize i)	const	{ ASSERT( i < 2 );  return _value.m128i_i64[i]; }
		ND_ EnableIf<isU64, IntType>	operator [] (usize i)	const	{ ASSERT( i < 2 );  return _value.m128i_u64[i]; }


		ND_ EnableIf<is8,  Self>	Add (const Self &rhs)	const	{ return Self{ _mm_add_epi8( _value, rhs._value )}; }
		ND_ EnableIf<is16, Self>	Add (const Self &rhs)	const	{ return Self{ _mm_add_epi16( _value, rhs._value )}; }
		ND_ EnableIf<is32, Self>	Add (const Self &rhs)	const	{ return Self{ _mm_add_epi32( _value, rhs._value )}; }
		ND_ EnableIf<is64, Self>	Add (const Self &rhs)	const	{ return Self{ _mm_add_epi64( _value, rhs._value )}; }
		
		ND_ EnableIf<is8,  Self>	Sub (const Self &rhs)	const	{ return Self{ _mm_sub_epi8( _value, rhs._value )}; }
		ND_ EnableIf<is16, Self>	Sub (const Self &rhs)	const	{ return Self{ _mm_sub_epi16( _value, rhs._value )}; }
		ND_ EnableIf<is32, Self>	Sub (const Self &rhs)	const	{ return Self{ _mm_sub_epi32( _value, rhs._value )}; }
		ND_ EnableIf<is64, Self>	Sub (const Self &rhs)	const	{ return Self{ _mm_sub_epi64( _value, rhs._value )}; }
		
		ND_ EnableIf<is32, Self>	Mul (const Self &rhs)	const	{ return Self{ _mm_mul_epu32( _value, rhs._value )}; }
		
		ND_ Self					And (const Self &rhs)	const	{ return Self{ _mm_and_si128( _value, rhs._value )}; }
		ND_ Self					Or  (const Self &rhs)	const	{ return Self{ _mm_or_si128( _value, rhs._value )}; }
		ND_ Self					Xor (const Self &rhs)	const	{ return Self{ _mm_xor_si128( _value, rhs._value )}; }
		ND_ Self					AndNot (const Self &rhs)const	{ return Self{ _mm_andnot_si128( _value, rhs._value )}; }	// !a & b
		

		ND_ EnableIf<isI8,  Self>	Min (const Self &rhs)	const	{ return Self{ _mm_min_epi8( _value, rhs._value )}; }
		ND_ EnableIf<isI16, Self>	Min (const Self &rhs)	const	{ return Self{ _mm_min_epi16( _value, rhs._value )}; }
		ND_ EnableIf<isI32, Self>	Min (const Self &rhs)	const	{ return Self{ _mm_sub_epi32( _value, rhs._value )}; }
		ND_ EnableIf<isI64, Self>	Min (const Self &rhs)	const	{ return Self{ _mm_sub_epi64( _value, rhs._value )}; }

		ND_ EnableIf<isU8,  Self>	Min (const Self &rhs)	const	{ return Self{ _mm_min_epu8( _value, rhs._value )}; }
		ND_ EnableIf<isU16, Self>	Min (const Self &rhs)	const	{ return Self{ _mm_min_epu16( _value, rhs._value )}; }
		ND_ EnableIf<isU32, Self>	Min (const Self &rhs)	const	{ return Self{ _mm_sub_epu32( _value, rhs._value )}; }
		ND_ EnableIf<isU64, Self>	Min (const Self &rhs)	const	{ return Self{ _mm_sub_epu64( _value, rhs._value )}; }
		
		ND_ EnableIf<isI8,  Self>	Max (const Self &rhs)	const	{ return Self{ _mm_max_epi8( _value, rhs._value )}; }
		ND_ EnableIf<isI16, Self>	Max (const Self &rhs)	const	{ return Self{ _mm_max_epi16( _value, rhs._value )}; }
		ND_ EnableIf<isI32, Self>	Max (const Self &rhs)	const	{ return Self{ _mm_max_epi32( _value, rhs._value )}; }
		ND_ EnableIf<isI64, Self>	Max (const Self &rhs)	const	{ return Self{ _mm_max_epi64( _value, rhs._value )}; }
		
		ND_ EnableIf<isU8,  Self>	Max (const Self &rhs)	const	{ return Self{ _mm_max_epu8( _value, rhs._value )}; }
		ND_ EnableIf<isU16, Self>	Max (const Self &rhs)	const	{ return Self{ _mm_max_epu16( _value, rhs._value )}; }
		ND_ EnableIf<isU32, Self>	Max (const Self &rhs)	const	{ return Self{ _mm_max_epu32( _value, rhs._value )}; }
		ND_ EnableIf<isU64, Self>	Max (const Self &rhs)	const	{ return Self{ _mm_max_epu64( _value, rhs._value )}; }
		

		// returns 0 or ~0
		ND_ EnableIf<is8,  Self>	Equal (const Self &rhs)		const	{ return Self{ _mm_cmpeq_epi8( _value, rhs._value )}; }
		ND_ EnableIf<is16,  Self>	Equal (const Self &rhs)		const	{ return Self{ _mm_cmpeq_epi16( _value, rhs._value )}; }
		ND_ EnableIf<is32,  Self>	Equal (const Self &rhs)		const	{ return Self{ _mm_cmpeq_epi32( _value, rhs._value )}; }
		ND_ EnableIf<isI64, Self>	Equal (const Self &rhs)		const	{ return Self{ _mm_cmpeq_epi64( _value, rhs._value )}; }

		ND_ EnableIf<isI8, Self>	Greater (const Self &rhs)	const	{ return Self{ _mm_cmpgt_epi8( _value, rhs._value )}; }
		ND_ EnableIf<isI16, Self>	Greater (const Self &rhs)	const	{ return Self{ _mm_cmpgt_epi16( _value, rhs._value )}; }
		ND_ EnableIf<isI32, Self>	Greater (const Self &rhs)	const	{ return Self{ _mm_cmpgt_epi32( _value, rhs._value )}; }
		ND_ EnableIf<isI64, Self>	Greater (const Self &rhs)	const	{ return Self{ _mm_cmpgt_epi64( _value, rhs._value )}; }

		ND_ EnableIf<isI8, Self>	Less (const Self &rhs)		const	{ return Self{ _mm_cmplt_epi8( _value, rhs._value )}; }
		ND_ EnableIf<isI16, Self>	Less (const Self &rhs)		const	{ return Self{ _mm_cmplt_epi16( _value, rhs._value )}; }
		ND_ EnableIf<isI32, Self>	Less (const Self &rhs)		const	{ return Self{ _mm_cmplt_epi32( _value, rhs._value )}; }
		ND_ EnableIf<isI64, Self>	Less (const Self &rhs)		const	{ return Self{ _mm_cmplt_epi64( _value, rhs._value )}; }


		// zero on overflow
		ND_ EnableIf<is16, Self>	LShift (int shift)			const	{ return Self{ _mm_slli_epi16( _value, shift )}; }
		ND_ EnableIf<is32, Self>	LShift (int shift)			const	{ return Self{ _mm_slli_epi32( _value, shift )}; }
		ND_ EnableIf<is64, Self>	LShift (int shift)			const	{ return Self{ _mm_slli_epi64( _value, shift )}; }
		ND_ EnableIf<is16, Self>	LShift (const Self &rhs)	const	{ return Self{ _mm_sll_epi16( _value, rhs._value )}; }
		ND_ EnableIf<is32, Self>	LShift (const Self &rhs)	const	{ return Self{ _mm_sll_epi32( _value, rhs._value )}; }
		ND_ EnableIf<is64, Self>	LShift (const Self &rhs)	const	{ return Self{ _mm_sll_epi64( _value, rhs._value )}; }
		
		// zero on overflow
		ND_ EnableIf<is16, Self>	RShift (int shift)			const	{ return Self{ _mm_srli_epi16( _value, shift )}; }
		ND_ EnableIf<is32, Self>	RShift (int shift)			const	{ return Self{ _mm_srli_epi32( _value, shift )}; }
		ND_ EnableIf<is64, Self>	RShift (int shift)			const	{ return Self{ _mm_srli_epi64( _value, shift )}; }
		ND_ EnableIf<is16, Self>	RShift (const Self &rhs)	const	{ return Self{ _mm_srl_epi16( _value, rhs._value )}; }
		ND_ EnableIf<is32, Self>	RShift (const Self &rhs)	const	{ return Self{ _mm_srl_epi32( _value, rhs._value )}; }
		ND_ EnableIf<is64, Self>	RShift (const Self &rhs)	const	{ return Self{ _mm_srl_epi64( _value, rhs._value )}; }
		
		// max on overflow
		ND_ EnableIf<is16, Self>	RShiftA (int shift)			const	{ return Self{ _mm_srai_epi16( _value, shift )}; }
		ND_ EnableIf<is32, Self>	RShiftA (int shift)			const	{ return Self{ _mm_srai_epi32( _value, shift )}; }
		ND_ EnableIf<is64, Self>	RShiftA (int shift)			const	{ return Self{ _mm_srai_epi64( _value, shift )}; }		// AVX512F + AVX512VL
		ND_ EnableIf<is16, Self>	RShiftA (const Self &rhs)	const	{ return Self{ _mm_sra_epi16( _value, rhs._value )}; }
		ND_ EnableIf<is32, Self>	RShiftA (const Self &rhs)	const	{ return Self{ _mm_sra_epi32( _value, rhs._value )}; }
		ND_ EnableIf<is64, Self>	RShiftA (const Self &rhs)	const	{ return Self{ _mm_sra_epi64( _value, rhs._value )}; }	// AVX512F + AVX512VL


			EnableIf<is8>			ToArray (OUT IntType* dst)	const	{ _mm_storeu_epi8( OUT static_cast<void*>(dst), _value ); }
			EnableIf<is16>			ToArray (OUT IntType* dst)	const	{ _mm_storeu_epi16( OUT static_cast<void*>(dst), _value ); }
			EnableIf<is32>			ToArray (OUT IntType* dst)	const	{ _mm_storeu_epi32( OUT static_cast<void*>(dst), _value ); }
			EnableIf<is64>			ToArray (OUT IntType* dst)	const	{ _mm_storeu_epi64( OUT static_cast<void*>(dst), _value ); }
			
		// TODO: AES

		template <typename IT>
		ND_ SimdTInt128<IT> &		Cast ()			{ return reinterpret_cast< SimdTInt128<IT> &>(*this); }

		template <typename IT>
		ND_ SimdTInt128<IT> const&	Cast ()	const	{ return reinterpret_cast< SimdTInt128<IT> const &>(*this); }

		ND_ Int128b const&			Cast ()	const	{ return reinterpret_cast< Int128b const &>(*this); }
	};
	

	template <typename IT>
	inline EnableIf< IsInteger<IT>, SimdTInt128<IT>>  SimdFloat4::Cast () const
	{
		return SimdTInt128<IT>{ _mm_castps_si128( _value )};
	}
		
	template <typename IT>
	inline EnableIf< IsInteger<IT>, SimdTInt128<IT>>  SimdDouble2::Cast () const
	{
		return SimdTInt128<IT>{ _mm_castpd_si128( _value )};
	}
	
	template <typename T>
	inline EnableIf< IsSameTypes<T, double>, SimdDouble2>  SimdFloat4::Cast () const
	{
		return SimdDouble2{ _mm_castps_pd( _value )};
	}

# endif // AE_SIMD_SSE
//-----------------------------------------------------------------------------


# ifdef AE_SIMD_AVX

	//
	// 256 bits float (AVX)
	//
	struct SimdFloat8
	{
	// types
	public:
		using value_type	= float;
		using Self			= SimdFloat8;
		

	// variables
	private:
		__m256		_value;		// float[8]


	// methods
	public:
		SimdFloat8 () : _value{ _mm256_setzero_ps() } {}
		explicit SimdFloat8 (float v) : _value{ _mm256_set1_ps( v )} {}
		explicit SimdFloat8 (const float *v) : _value{ _mm256_loadu_ps( v )} {}
		explicit SimdFloat8 (const __m256 &v) : _value{v} {}

		SimdFloat8 (float v0, float v1, float v2, float v3,
					float v4, float v5, float v6, float v7) :
			_value{ _mm256_set_ps( v0, v1, v2, v3, v4, v5, v6, v7 )} {}

		ND_ SimdFloat8	operator +  (const SimdFloat8 &rhs)	const	{ return Add( rhs ); }
		ND_ SimdFloat8	operator -  (const SimdFloat8 &rhs)	const	{ return Sub( rhs ); }
		ND_ SimdFloat8	operator *  (const SimdFloat8 &rhs)	const	{ return Mul( rhs ); }
		ND_ SimdFloat8	operator /  (const SimdFloat8 &rhs)	const	{ return Div( rhs ); }
		
		ND_ SimdFloat8	operator &  (const SimdFloat8 &rhs)	const	{ return And( rhs ); }
		ND_ SimdFloat8	operator |  (const SimdFloat8 &rhs)	const	{ return Or( rhs ); }
		ND_ SimdFloat8	operator ^  (const SimdFloat8 &rhs)	const	{ return Xor( rhs ); }

		ND_ SimdFloat8	operator == (const SimdFloat8 &rhs)	const	{ return Equal( rhs ); }
		ND_ SimdFloat8	operator != (const SimdFloat8 &rhs)	const	{ return NotEqual( rhs ); }
		ND_ SimdFloat8	operator >  (const SimdFloat8 &rhs)	const	{ return Greater( rhs ); }
		ND_ SimdFloat8	operator <  (const SimdFloat8 &rhs)	const	{ return Less( rhs ); }
		ND_ SimdFloat8	operator >= (const SimdFloat8 &rhs)	const	{ return GEqual( rhs ); }
		ND_ SimdFloat8	operator <= (const SimdFloat8 &rhs)	const	{ return LEqual( rhs ); }

		ND_ float		operator [] (usize i)				const	{ ASSERT( i < 8 ); return _value.m256_f32[i]; }


		ND_ SimdFloat8	Add (const SimdFloat8 &rhs)			const	{ return SimdFloat8{ _mm256_add_ps( _value, rhs._value )}; }
		ND_ SimdFloat8	Sub (const SimdFloat8 &rhs)			const	{ return SimdFloat8{ _mm256_sub_ps( _value, rhs._value )}; }
		ND_ SimdFloat8	Mul (const SimdFloat8 &rhs)			const	{ return SimdFloat8{ _mm256_mul_ps( _value, rhs._value )}; }
		ND_ SimdFloat8	Div (const SimdFloat8 &rhs)			const	{ return SimdFloat8{ _mm256_div_ps( _value, rhs._value )}; }
		ND_ SimdFloat8	Min (const SimdFloat8 &rhs)			const	{ return SimdFloat8{ _mm256_min_ps( _value, rhs._value )}; }
		ND_ SimdFloat8	Max (const SimdFloat8 &rhs)			const	{ return SimdFloat8{ _mm256_max_ps( _value, rhs._value )}; }
		
		ND_ SimdFloat8	And (const SimdFloat8 &rhs)			const	{ return SimdFloat8{ _mm256_and_ps( _value, rhs._value )}; }
		ND_ SimdFloat8	Or  (const SimdFloat8 &rhs)			const	{ return SimdFloat8{ _mm256_or_ps( _value, rhs._value )}; }
		ND_ SimdFloat8	Xor (const SimdFloat8 &rhs)			const	{ return SimdFloat8{ _mm256_xor_ps( _value, rhs._value )}; }
		ND_ SimdFloat8	AndNot (const SimdFloat8 &rhs)		const	{ return SimdFloat8{ _mm256_andnot_ps( _value, rhs._value )}; }	// !a & b

		// returns { a[0] + a[1],  a[2] + a[3],  b[0] + b[1],  b[2] + b[3] }
		ND_ SimdFloat8	HAdd (const SimdFloat8 &rhs)		const	{ return SimdFloat8{ _mm256_hadd_ps( _value, rhs._value )}; }		// SSE 3
		
		// returns { a[0] - b[0], a[1] + b[1], a[2] - b[2], a[3] + b[3] }
		ND_ SimdFloat8	AddSub (const SimdFloat8 &rhs)		const	{ return SimdFloat8{ _mm256_addsub_ps( _value, rhs._value )}; }	// SSE 3

		ND_ SimdFloat8	Sqrt ()								const	{ return SimdFloat8{ _mm256_sqrt_ps( _value )}; }
		ND_ SimdFloat8	Reciporal ()						const	{ return SimdFloat8{ _mm256_rcp_ps( _value )}; }		// 1 / x
		ND_ SimdFloat8	RSqrt ()							const	{ return SimdFloat8{ _mm256_rsqrt_ps( _value )}; }		// 1 / sqrt(x)
		
		// returns 0 or ~0u
		// ordered - comparison with NaN returns false
		ND_ SimdFloat8	Equal    (const SimdFloat8 &rhs)	const	{ return SimdFloat8{ _mm256_cmp_ps( _value, rhs._value, _CMP_EQ_OQ  )}; }
		ND_ SimdFloat8	NotEqual (const SimdFloat8 &rhs)	const	{ return SimdFloat8{ _mm256_cmp_ps( _value, rhs._value, _CMP_NEQ_OQ )}; }
		ND_ SimdFloat8	Greater  (const SimdFloat8 &rhs)	const	{ return SimdFloat8{ _mm256_cmp_ps( _value, rhs._value, _CMP_GT_OQ  )}; }
		ND_ SimdFloat8	Less     (const SimdFloat8 &rhs)	const	{ return SimdFloat8{ _mm256_cmp_ps( _value, rhs._value, _CMP_LT_OQ  )}; }
		ND_ SimdFloat8	GEqual   (const SimdFloat8 &rhs)	const	{ return SimdFloat8{ _mm256_cmp_ps( _value, rhs._value, _CMP_GE_OQ  )}; }
		ND_ SimdFloat8	LEqual   (const SimdFloat8 &rhs)	const	{ return SimdFloat8{ _mm256_cmp_ps( _value, rhs._value, _CMP_LE_OQ  )}; }
		
		// returns 0 or ~0u
		// unordered - comparison with NaN returns true
		ND_ SimdFloat8	EqualU    (const SimdFloat8 &rhs)	const	{ return SimdFloat8{ _mm256_cmp_ps( _value, rhs._value, _CMP_EQ_UQ  )}; }
		ND_ SimdFloat8	NotEqualU (const SimdFloat8 &rhs)	const	{ return SimdFloat8{ _mm256_cmp_ps( _value, rhs._value, _CMP_NEQ_UQ )}; }
		ND_ SimdFloat8	GreaterU  (const SimdFloat8 &rhs)	const	{ return SimdFloat8{ _mm256_cmp_ps( _value, rhs._value, _CMP_NLE_UQ )}; }
		ND_ SimdFloat8	LessU     (const SimdFloat8 &rhs)	const	{ return SimdFloat8{ _mm256_cmp_ps( _value, rhs._value, _CMP_NGE_UQ )}; }
		ND_ SimdFloat8	GEqualU   (const SimdFloat8 &rhs)	const	{ return SimdFloat8{ _mm256_cmp_ps( _value, rhs._value, _CMP_NLT_UQ )}; }
		ND_ SimdFloat8	LEqualU   (const SimdFloat8 &rhs)	const	{ return SimdFloat8{ _mm256_cmp_ps( _value, rhs._value, _CMP_NGT_UQ )}; }
		
		ND_ SimdFloat8	DotV (const SimdFloat8 &rhs)		const	{ return SimdFloat8{ _mm256_dp_ps( _value, rhs._value, 0xFF )}; }
		
			void ToArray (OUT float* dst)					const	{ _mm256_storeu_ps( OUT dst, _value ); }
	};
	


	//
	// 256 bits double (AVX 2)
	//
	struct SimdDouble4
	{
	// types
	public:
		using value_type	= double;
		using Self			= SimdDouble4;
		

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

		ND_ SimdDouble4	operator + (const SimdDouble4 &rhs)		const	{ return Add( rhs ); }
		ND_ SimdDouble4	operator - (const SimdDouble4 &rhs)		const	{ return Sub( rhs ); }
		ND_ SimdDouble4	operator * (const SimdDouble4 &rhs)		const	{ return Mul( rhs ); }
		ND_ SimdDouble4	operator / (const SimdDouble4 &rhs)		const	{ return Div( rhs ); }
		
		ND_ SimdDouble4	operator &  (const SimdDouble4 &rhs)	const	{ return And( rhs ); }
		ND_ SimdDouble4	operator |  (const SimdDouble4 &rhs)	const	{ return Or( rhs ); }
		ND_ SimdDouble4	operator ^  (const SimdDouble4 &rhs)	const	{ return Xor( rhs ); }

		ND_ SimdDouble4	operator == (const SimdDouble4 &rhs)	const	{ return Equal( rhs ); }
		ND_ SimdDouble4	operator != (const SimdDouble4 &rhs)	const	{ return NotEqual( rhs ); }
		ND_ SimdDouble4	operator >  (const SimdDouble4 &rhs)	const	{ return Greater( rhs ); }
		ND_ SimdDouble4	operator <  (const SimdDouble4 &rhs)	const	{ return Less( rhs ); }
		ND_ SimdDouble4	operator >= (const SimdDouble4 &rhs)	const	{ return GEqual( rhs ); }
		ND_ SimdDouble4	operator <= (const SimdDouble4 &rhs)	const	{ return LEqual( rhs ); }
		
		ND_ double		operator [] (usize i)					const	{ ASSERT( i < 4 ); return _value.m256d_f64[i]; }


		ND_ SimdDouble4	Add (const SimdDouble4 &rhs)			const	{ return SimdDouble4{ _mm256_add_pd( _value, rhs._value )}; }
		ND_ SimdDouble4	Sub (const SimdDouble4 &rhs)			const	{ return SimdDouble4{ _mm256_sub_pd( _value, rhs._value )}; }
		ND_ SimdDouble4	Mul (const SimdDouble4 &rhs)			const	{ return SimdDouble4{ _mm256_mul_pd( _value, rhs._value )}; }
		ND_ SimdDouble4	Div (const SimdDouble4 &rhs)			const	{ return SimdDouble4{ _mm256_div_pd( _value, rhs._value )}; }
		ND_ SimdDouble4	Min (const SimdDouble4 &rhs)			const	{ return SimdDouble4{ _mm256_min_pd( _value, rhs._value )}; }
		ND_ SimdDouble4	Max (const SimdDouble4 &rhs)			const	{ return SimdDouble4{ _mm256_max_pd( _value, rhs._value )}; }
		
		ND_ SimdDouble4	And (const SimdDouble4 &rhs)			const	{ return SimdDouble4{ _mm256_and_pd( _value, rhs._value )}; }
		ND_ SimdDouble4	Or  (const SimdDouble4 &rhs)			const	{ return SimdDouble4{ _mm256_or_pd( _value, rhs._value )}; }
		ND_ SimdDouble4	Xor (const SimdDouble4 &rhs)			const	{ return SimdDouble4{ _mm256_xor_pd( _value, rhs._value )}; }
		ND_ SimdDouble4	AndNot (const SimdDouble4 &rhs)			const	{ return SimdDouble4{ _mm256_andnot_pd( _value, rhs._value )}; }	// !a & b

		// returns { a[0] + a[1],  a[2] + a[3],  b[0] + b[1],  b[2] + b[3] }
		ND_ SimdDouble4	HAdd (const SimdDouble4 &rhs)			const	{ return SimdDouble4{ _mm256_hadd_pd( _value, rhs._value )}; }
		
		// returns { a[0] - b[0], a[1] + b[1], a[2] - b[2], a[3] + b[3] }
		ND_ SimdDouble4	AddSub (const SimdDouble4 &rhs)			const	{ return SimdDouble4{ _mm256_addsub_pd( _value, rhs._value )}; }

		ND_ SimdDouble4	Sqrt ()									const	{ return SimdDouble4{ _mm256_sqrt_pd( _value )}; }

		// returns 0 or ~0ull
		// ordered - comparison with NaN returns false
		ND_ SimdDouble4	Equal    (const SimdDouble4 &rhs)		const	{ return SimdDouble4{ _mm256_cmp_pd( _value, rhs._value, _CMP_EQ_OQ  )}; }
		ND_ SimdDouble4	NotEqual (const SimdDouble4 &rhs)		const	{ return SimdDouble4{ _mm256_cmp_pd( _value, rhs._value, _CMP_NEQ_OQ )}; }
		ND_ SimdDouble4	Greater  (const SimdDouble4 &rhs)		const	{ return SimdDouble4{ _mm256_cmp_pd( _value, rhs._value, _CMP_GT_OQ  )}; }
		ND_ SimdDouble4	Less     (const SimdDouble4 &rhs)		const	{ return SimdDouble4{ _mm256_cmp_pd( _value, rhs._value, _CMP_LT_OQ  )}; }
		ND_ SimdDouble4	GEqual   (const SimdDouble4 &rhs)		const	{ return SimdDouble4{ _mm256_cmp_pd( _value, rhs._value, _CMP_GE_OQ  )}; }
		ND_ SimdDouble4	LEqual   (const SimdDouble4 &rhs)		const	{ return SimdDouble4{ _mm256_cmp_pd( _value, rhs._value, _CMP_LE_OQ  )}; }
		
		// returns 0 or ~0ull
		// unordered - comparison with NaN returns true
		ND_ SimdDouble4	EqualU    (const SimdDouble4 &rhs)		const	{ return SimdDouble4{ _mm256_cmp_pd( _value, rhs._value, _CMP_EQ_UQ  )}; }
		ND_ SimdDouble4	NotEqualU (const SimdDouble4 &rhs)		const	{ return SimdDouble4{ _mm256_cmp_pd( _value, rhs._value, _CMP_NEQ_UQ )}; }
		ND_ SimdDouble4	GreaterU  (const SimdDouble4 &rhs)		const	{ return SimdDouble4{ _mm256_cmp_pd( _value, rhs._value, _CMP_NLE_UQ )}; }
		ND_ SimdDouble4	LessU     (const SimdDouble4 &rhs)		const	{ return SimdDouble4{ _mm256_cmp_pd( _value, rhs._value, _CMP_NGE_UQ )}; }
		ND_ SimdDouble4	GEqualU   (const SimdDouble4 &rhs)		const	{ return SimdDouble4{ _mm256_cmp_pd( _value, rhs._value, _CMP_NLT_UQ )}; }
		ND_ SimdDouble4	LEqualU   (const SimdDouble4 &rhs)		const	{ return SimdDouble4{ _mm256_cmp_pd( _value, rhs._value, _CMP_NGT_UQ )}; }

		
			void ToArray (OUT double* dst)						const	{ _mm256_storeu_pd( OUT dst, _value ); }
	};



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
		
		ND_ Self	operator & (const Self &rhs)	const	{ return And( rhs ); }
		ND_ Self	operator | (const Self &rhs)	const	{ return Or( rhs ); }
		ND_ Self	operator ^ (const Self &rhs)	const	{ return Xor( rhs ); }
		
		ND_ Self	And (const Self &rhs)	const	{ return Self{ _mm256_and_si256( _value, rhs._value )}; }
		ND_ Self	Or  (const Self &rhs)	const	{ return Self{ _mm256_or_si256( _value, rhs._value )}; }
		ND_ Self	Xor (const Self &rhs)	const	{ return Self{ _mm256_xor_si256( _value, rhs._value )}; }
		ND_ Self	AndNot (const Self &rhs)const	{ return Self{ _mm256_andnot_si256( _value, rhs._value )}; }	// !a & b
		
		// shift in bytes
		template <int ShiftBytes>	ND_ Self	LShiftB ()	const	{ return Self{ _mm256_slli_si256( _value, ShiftBytes )}; }
		template <int ShiftBytes>	ND_ Self	RShiftB ()	const	{ return Self{ _mm256_srli_si256( _value, ShiftBytes )}; }

		ND_ int		ToInt32 ()						const	{ return _mm256_cvtsi256_si32( _value ); }
		ND_ slong	ToInt64 ()						const	{ return _mm256_cvtsi256_si64( _value ); }
		ND_ Int128b	ToInt128 ()						const	{ return Int128b{ _mm256_castsi256_si128( _value )}; }
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
		using value_type	= IntType;
		using Self			= SimdTInt256< IntType >;


	// variables
	private:
		__m256i		_value;		// int128[2], int64[4], int32[8], int16[16], int8[32]
		
		static constexpr bool	is8   = sizeof( IntType ) == 1;
		static constexpr bool	is16  = sizeof( IntType ) == 2;
		static constexpr bool	is32  = sizeof( IntType ) == 4;
		static constexpr bool	is64  = sizeof( IntType ) == 8;
		static constexpr bool	is128 = sizeof( IntType ) == 16;
		
		static constexpr bool	isU8  = IsSameTypes< IntType, sbyte >;
		static constexpr bool	isU16 = IsSameTypes< IntType, short >;
		static constexpr bool	isU32 = IsSameTypes< IntType, int >;
		static constexpr bool	isU64 = IsSameTypes< IntType, slong >;
		
		static constexpr bool	isI8  = IsSameTypes< IntType, ubyte >;
		static constexpr bool	isI16 = IsSameTypes< IntType, ushort >;
		static constexpr bool	isI32 = IsSameTypes< IntType, uint >;
		static constexpr bool	isI64 = IsSameTypes< IntType, ulong >;


	// methods
	public:
		SimdTInt256 () : _value{ _mm256_setzero_si256()} {}
		explicit SimdTInt256 (__m256i v) : _value{ v } {}
		
		explicit SimdTInt256 (IntType v)
		{
			if constexpr( is8 )		_value = _mm256_set1_epi8( v );		else
			if constexpr( is16 )	_value = _mm256_set1_epi16( v );	else
			if constexpr( is32 )	_value = _mm256_set1_epi32( v );	else
			if constexpr( is64 )	_value = _mm256_set1_epi64x( v );	else
			if constexpr( is128 )	_value = _mm256_set_m128( v, v );	else
									_value = v; // compilation error
		}

		explicit SimdTInt256 (const IntType* v)
		{
			if constexpr( is8 )		_value = _mm256_loadu_epi8( v );	else
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

		ND_ Self	operator == (const Self &rhs)	const	{ return Equal( rhs ); }
		ND_ Self	operator != (const Self &rhs)	const	{ return NotEqual( rhs ); }
		ND_ Self	operator >  (const Self &rhs)	const	{ return Greater( rhs ); }
		ND_ Self	operator <  (const Self &rhs)	const	{ return Less( rhs ); }
		ND_ Self	operator >= (const Self &rhs)	const	{ return GEqual( rhs ); }
		ND_ Self	operator <= (const Self &rhs)	const	{ return LEqual( rhs ); }
		
		
		ND_ EnableIf<isI8,  IntType>	operator [] (usize i)	const	{ ASSERT( i < 32 ); return _value.m256i_i8[i]; }
		ND_ EnableIf<isU8,  IntType>	operator [] (usize i)	const	{ ASSERT( i < 32 ); return _value.m256i_u8[i]; }
		ND_ EnableIf<isI16, IntType>	operator [] (usize i)	const	{ ASSERT( i < 16 ); return _value.m256i_i16[i]; }
		ND_ EnableIf<isU16, IntType>	operator [] (usize i)	const	{ ASSERT( i < 16 ); return _value.m256i_u16[i]; }
		ND_ EnableIf<isI32, IntType>	operator [] (usize i)	const	{ ASSERT( i < 8 );  return _value.m256i_i32[i]; }
		ND_ EnableIf<isU32, IntType>	operator [] (usize i)	const	{ ASSERT( i < 8 );  return _value.m256i_u32[i]; }
		ND_ EnableIf<isI64, IntType>	operator [] (usize i)	const	{ ASSERT( i < 4 );  return _value.m256i_i64[i]; }
		ND_ EnableIf<isU64, IntType>	operator [] (usize i)	const	{ ASSERT( i < 4 );  return _value.m256i_u64[i]; }
		ND_ EnableIf<is128, IntType>	operator [] (usize i)	const	{ ASSERT( i < 2 );  return IntType{ _mm256_extractf128_si256( _value, i )}; }


		ND_ EnableIf<is8,  Self>	Add (const Self &rhs)	const	{ return Self{ _mm256_add_epi8( _value, rhs._value )}; }
		ND_ EnableIf<is16, Self>	Add (const Self &rhs)	const	{ return Self{ _mm256_add_epi16( _value, rhs._value )}; }
		ND_ EnableIf<is32, Self>	Add (const Self &rhs)	const	{ return Self{ _mm256_add_epi32( _value, rhs._value )}; }
		ND_ EnableIf<is64, Self>	Add (const Self &rhs)	const	{ return Self{ _mm256_add_epi64( _value, rhs._value )}; }
		
		ND_ EnableIf<is8,  Self>	Sub (const Self &rhs)	const	{ return Self{ _mm256_sub_epi8( _value, rhs._value )}; }
		ND_ EnableIf<is16, Self>	Sub (const Self &rhs)	const	{ return Self{ _mm256_sub_epi16( _value, rhs._value )}; }
		ND_ EnableIf<is32, Self>	Sub (const Self &rhs)	const	{ return Self{ _mm256_sub_epi32( _value, rhs._value )}; }
		ND_ EnableIf<is64, Self>	Sub (const Self &rhs)	const	{ return Self{ _mm256_sub_epi64( _value, rhs._value )}; }
		
		ND_ EnableIf<is32, Self>	Mul (const Self &rhs)	const	{ return Self{ _mm256_mul_epu32( _value, rhs._value )}; }
		
		ND_ Self					And (const Self &rhs)	const	{ return Self{ _mm256_and_si256( _value, rhs._value )}; }
		ND_ Self					Or  (const Self &rhs)	const	{ return Self{ _mm256_or_si256( _value, rhs._value )}; }
		ND_ Self					Xor (const Self &rhs)	const	{ return Self{ _mm256_xor_si256( _value, rhs._value )}; }
		ND_ Self					AndNot (const Self &rhs)const	{ return Self{ _mm256_andnot_si256( _value, rhs._value )}; }	// !a & b
		

		ND_ EnableIf<isI8,  Self>	Min (const Self &rhs)	const	{ return Self{ _mm256_min_epi8( _value, rhs._value )}; }
		ND_ EnableIf<isI16, Self>	Min (const Self &rhs)	const	{ return Self{ _mm256_min_epi16( _value, rhs._value )}; }
		ND_ EnableIf<isI32, Self>	Min (const Self &rhs)	const	{ return Self{ _mm256_sub_epi32( _value, rhs._value )}; }
		ND_ EnableIf<isI64, Self>	Min (const Self &rhs)	const	{ return Self{ _mm256_sub_epi64( _value, rhs._value )}; }

		ND_ EnableIf<isU8,  Self>	Min (const Self &rhs)	const	{ return Self{ _mm256_min_epu8( _value, rhs._value )}; }
		ND_ EnableIf<isU16, Self>	Min (const Self &rhs)	const	{ return Self{ _mm256_min_epu16( _value, rhs._value )}; }
		ND_ EnableIf<isU32, Self>	Min (const Self &rhs)	const	{ return Self{ _mm256_sub_epu32( _value, rhs._value )}; }
		ND_ EnableIf<isU64, Self>	Min (const Self &rhs)	const	{ return Self{ _mm256_sub_epu64( _value, rhs._value )}; }
		
		ND_ EnableIf<isI8,  Self>	Max (const Self &rhs)	const	{ return Self{ _mm256_max_epi8( _value, rhs._value )}; }
		ND_ EnableIf<isI16, Self>	Max (const Self &rhs)	const	{ return Self{ _mm256_max_epi16( _value, rhs._value )}; }
		ND_ EnableIf<isI32, Self>	Max (const Self &rhs)	const	{ return Self{ _mm256_max_epi32( _value, rhs._value )}; }
		ND_ EnableIf<isI64, Self>	Max (const Self &rhs)	const	{ return Self{ _mm256_max_epi64( _value, rhs._value )}; }
		
		ND_ EnableIf<isU8,  Self>	Max (const Self &rhs)	const	{ return Self{ _mm256_max_epu8( _value, rhs._value )}; }
		ND_ EnableIf<isU16, Self>	Max (const Self &rhs)	const	{ return Self{ _mm256_max_epu16( _value, rhs._value )}; }
		ND_ EnableIf<isU32, Self>	Max (const Self &rhs)	const	{ return Self{ _mm256_max_epu32( _value, rhs._value )}; }
		ND_ EnableIf<isU64, Self>	Max (const Self &rhs)	const	{ return Self{ _mm256_max_epu64( _value, rhs._value )}; }
		

		// returns 0 or ~0
		ND_ EnableIf<is8,  Self>	Equal (const Self &rhs)		const	{ return Self{ _mm256_cmpeq_epi8( _value, rhs._value )}; }
		ND_ EnableIf<is16,  Self>	Equal (const Self &rhs)		const	{ return Self{ _mm256_cmpeq_epi16( _value, rhs._value )}; }
		ND_ EnableIf<is32,  Self>	Equal (const Self &rhs)		const	{ return Self{ _mm256_cmpeq_epi32( _value, rhs._value )}; }
		ND_ EnableIf<isI64, Self>	Equal (const Self &rhs)		const	{ return Self{ _mm256_cmpeq_epi64( _value, rhs._value )}; }

		ND_ EnableIf<isI8, Self>	Greater (const Self &rhs)	const	{ return Self{ _mm256_cmpgt_epi8( _value, rhs._value )}; }
		ND_ EnableIf<isI16, Self>	Greater (const Self &rhs)	const	{ return Self{ _mm256_cmpgt_epi16( _value, rhs._value )}; }
		ND_ EnableIf<isI32, Self>	Greater (const Self &rhs)	const	{ return Self{ _mm256_cmpgt_epi32( _value, rhs._value )}; }
		ND_ EnableIf<isI64, Self>	Greater (const Self &rhs)	const	{ return Self{ _mm256_cmpgt_epi64( _value, rhs._value )}; }

		ND_ EnableIf<isI8, Self>	Less (const Self &rhs)		const	{ return Self{ _mm256_cmplt_epi8( _value, rhs._value )}; }
		ND_ EnableIf<isI16, Self>	Less (const Self &rhs)		const	{ return Self{ _mm256_cmplt_epi16( _value, rhs._value )}; }
		ND_ EnableIf<isI32, Self>	Less (const Self &rhs)		const	{ return Self{ _mm256_cmplt_epi32( _value, rhs._value )}; }
		ND_ EnableIf<isI64, Self>	Less (const Self &rhs)		const	{ return Self{ _mm256_cmplt_epi64( _value, rhs._value )}; }


		// zero on overflow
		ND_ EnableIf<is16, Self>	LShift (int shift)			const	{ return Self{ _mm256_slli_epi16( _value, shift )}; }
		ND_ EnableIf<is32, Self>	LShift (int shift)			const	{ return Self{ _mm256_slli_epi32( _value, shift )}; }
		ND_ EnableIf<is64, Self>	LShift (int shift)			const	{ return Self{ _mm256_slli_epi64( _value, shift )}; }
		ND_ EnableIf<is16, Self>	LShift (const Self &rhs)	const	{ return Self{ _mm256_sll_epi16( _value, rhs._value )}; }
		ND_ EnableIf<is32, Self>	LShift (const Self &rhs)	const	{ return Self{ _mm256_sll_epi32( _value, rhs._value )}; }
		ND_ EnableIf<is64, Self>	LShift (const Self &rhs)	const	{ return Self{ _mm256_sll_epi64( _value, rhs._value )}; }
		
		// zero on overflow
		ND_ EnableIf<is16, Self>	RShift (int shift)			const	{ return Self{ _mm256_srli_epi16( _value, shift )}; }
		ND_ EnableIf<is32, Self>	RShift (int shift)			const	{ return Self{ _mm256_srli_epi32( _value, shift )}; }
		ND_ EnableIf<is64, Self>	RShift (int shift)			const	{ return Self{ _mm256_srli_epi64( _value, shift )}; }
		ND_ EnableIf<is16, Self>	RShift (const Self &rhs)	const	{ return Self{ _mm256_srl_epi16( _value, rhs._value )}; }
		ND_ EnableIf<is32, Self>	RShift (const Self &rhs)	const	{ return Self{ _mm256_srl_epi32( _value, rhs._value )}; }
		ND_ EnableIf<is64, Self>	RShift (const Self &rhs)	const	{ return Self{ _mm256_srl_epi64( _value, rhs._value )}; }
		
		// max on overflow
		ND_ EnableIf<is16, Self>	RShiftA (int shift)			const	{ return Self{ _mm256_srai_epi16( _value, shift )}; }
		ND_ EnableIf<is32, Self>	RShiftA (int shift)			const	{ return Self{ _mm256_srai_epi32( _value, shift )}; }
		ND_ EnableIf<is64, Self>	RShiftA (int shift)			const	{ return Self{ _mm256_srai_epi64( _value, shift )}; }
		ND_ EnableIf<is16, Self>	RShiftA (const Self &rhs)	const	{ return Self{ _mm256_sra_epi16( _value, rhs._value )}; }
		ND_ EnableIf<is32, Self>	RShiftA (const Self &rhs)	const	{ return Self{ _mm256_sra_epi32( _value, rhs._value )}; }
		ND_ EnableIf<is64, Self>	RShiftA (const Self &rhs)	const	{ return Self{ _mm256_sra_epi64( _value, rhs._value )}; }
		
			EnableIf<is8>			ToArray (OUT IntType* dst)	const	{ _mm256_storeu_epi8( OUT static_cast<void*>(dst), _value ); }
			EnableIf<is16>			ToArray (OUT IntType* dst)	const	{ _mm256_storeu_epi16( OUT static_cast<void*>(dst), _value ); }
			EnableIf<is32>			ToArray (OUT IntType* dst)	const	{ _mm256_storeu_epi32( OUT static_cast<void*>(dst), _value ); }
			EnableIf<is64>			ToArray (OUT IntType* dst)	const	{ _mm256_storeu_epi64( OUT static_cast<void*>(dst), _value ); }

		// TODO: AES

		template <typename IT>
		ND_ SimdTInt256<IT> &		Cast ()			{ return reinterpret_cast< SimdTInt256<IT> &>(*this); }

		template <typename IT>
		ND_ SimdTInt256<IT> const&	Cast ()	const	{ return reinterpret_cast< SimdTInt256<IT> const &>(*this); }
		
		ND_ Int256b const&			Cast ()	const	{ return reinterpret_cast< Int256b const &>(*this); }
	};

	

	/*
	//
	// 128 bit half (AVX512_FP16 + AVX512VL)
	//
	struct SimdHalf8
	{
	// types
	public:
		using value_type	= glm::detail::hdata;
		using Self			= SimdHalf8;

		
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
		using value_type	= glm::detail::hdata;
		using Self			= SimdHalf16;

		
	// variables
	private:
		__m256		_value;		// half[16]
		

	// methods
	public:
		SimdHalf16 () : _value{_mm256_setzero_ps()}	{}
	};
	*/

# endif // AE_SIMD_AVX
//-----------------------------------------------------------------------------


# if 0 //def AE_SIMD_NEON

	//
	// 128 bit float (Neon)
	//
	struct SimdFloat4
	{
	// types
	public:
		using value_type	= float;
		using Self			= SimdFloat4;


	// variables
	private:
		float32x4_t	_value;		// float[4]


	// methods
	public:
		SimdFloat4 () : _value{}	{}
	};
	


	//
	// 128 bit int (Neon)
	//
	struct SimdInt4
	{
	// types
	public:
		using value_type	= float;
		using Self			= SimdInt4;


	// variables
	private:
		int32x4_t	_value;		// int[4]


	// methods
	public:
		SimdInt4 () : _value{}	{}
	};
	


	//
	// 128 bit uint (Neon)
	//
	struct SimdUInt4
	{
	// types
	public:
		using value_type	= float;
		using Self			= SimdUInt4;


	// variables
	private:
		uint32x4_t	_value;		// uint[4]


	// methods
	public:
		SimdUInt4 () : _value{}	{}
	};

# endif // AE_SIMD_NEON
//-----------------------------------------------------------------------------


# ifdef AE_SIMD_AVX512

	//
	// 512 bit float (AVX512)
	//
	struct SimdFloat16
	{
	// types
	public:
		using value_type	= float;
		using Self			= SimdFloat16;


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
		using value_type	= double;
		using Self			= SimdDouble8;


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
		using value_type	= IntType;
		using Self			= SimdTInt512< IntType >;


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

# endif // AE_SIMD_AVX512
//-----------------------------------------------------------------------------


} // AE::Math
