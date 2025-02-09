// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	512 bit SIMD
*/

#pragma once

namespace AE::Base
{
#if AE_SIMD_AVX >= 3

	//
	// 256 bit half (F16C, AVX512_FP16, AVX512VL)
	//
	#define AE_SIMD_SimdHalf16
	struct SimdHalf16
	{
	// types
	public:
		static constexpr uint	count = 16;

		using Scalar_t		= half;
		using Self			= SimdHalf16;
		using Native_t		= __m256h;
		using Array_t		= StaticArray< Scalar_t, count >;
		using Ptr_t			= AlignedPtr< sizeof(Native_t) >;
		using SimdInt_t		= SimdShort16;
		using SimdUInt_t	= SimdUShort16;

		StaticAssert( sizeof(Array_t) == sizeof(Native_t) );


		struct Bool16
		{
		private:
			__m256i		_value;

		public:
			explicit Bool16 (bool val)						__NE___	: _value{ _mm256_set1_epi16( val ? -1 : 0 )} {}
			explicit Bool16 (__m256i val)					__NE___	: _value{ val } {}
			explicit Bool16 (const SimdInt_t &v)			__NE___;
			explicit Bool16 (const SimdUInt_t &v)			__NE___;

			ND_ Bool16	operator | (const Bool16 &rhs)		C_NE___	{ return Bool16{ _mm256_or_si256( _value, rhs._value )}; }
			ND_ Bool16	operator & (const Bool16 &rhs)		C_NE___	{ return Bool16{ _mm256_and_si256( _value, rhs._value )}; }
			ND_ Bool16	operator ^ (const Bool16 &rhs)		C_NE___	{ return Bool16{ _mm256_xor_si256( _value, rhs._value )}; }
			ND_ Bool16	operator ! ()						C_NE___	{ return Bool16{ _mm256_andnot_si256( _value, _mm256_set1_epi16(-1) )}; }

			ND_ bool	All ()								C_NE___	{ return _mm256_movemask_epi8( _value ) == 0xFFFF; }
			ND_ bool	Any ()								C_NE___	{ return _mm256_movemask_epi8( _value ) != 0; }
			ND_ bool	None ()								C_NE___	{ return _mm256_movemask_epi8( _value ) == 0; }

			ND_ auto&	Ref ()								C_NE___	{ return _value; }

			ND_ explicit operator SimdInt_t ()				C_NE___	{ return SimdInt_t{ _value }; }
			ND_ explicit operator SimdUInt_t ()				C_NE___	{ return SimdUInt_t{ _value }; }
		};

	// variables
	private:
		__m256h		_value;		// half[16]


	// methods
	public:
		SimdHalf16 ()									__NE___	: _value{_mm256_setzero_si256()}	{}
		SimdHalf16 (Zero_t)								__NE___	: _value{ _mm256_setzero_si256() } {}
		explicit SimdHalf16 (Scalar_t v)				__NE___	: _value{ _mm256_set1_epi16( Base::BitCast<short>( v ))} {}
		explicit SimdHalf16 (const Native_t &v)			__NE___	: _value{ v } {}
		explicit SimdHalf16 (const Scalar_t* ptr)		__NE___	: _value{ _mm256_loadu_si256( reinterpret_cast<Native_t const *>( GetNonNull( ptr )) )} {}
		explicit SimdHalf16 (const Ptr_t ptr)			__NE___	: _value{ _mm256_load_si256( ptr.Cast<Native_t>() )} {}

		explicit SimdHalf16 (const SimdFloat8 &low)	__NE___;
		SimdHalf16 (const SimdFloat8 &low, const SimdFloat8 &high)	__NE___;
		SimdHalf16 (const SimdHalf8 &low, const SimdHalf8 &high)	__NE___;

		SimdHalf16 (Scalar_t v00, Scalar_t v01, Scalar_t v02, Scalar_t v03,
					Scalar_t v04, Scalar_t v05, Scalar_t v06, Scalar_t v07,
					Scalar_t v08, Scalar_t v09, Scalar_t v10, Scalar_t v11,
					Scalar_t v12, Scalar_t v13, Scalar_t v14, Scalar_t v15)	__NE___;
		SimdHalf16 (float v00, float v01, float v02, float v03,
					float v04, float v05, float v06, float v07,
					float v08, float v09, float v10, float v11,
					float v12, float v13, float v14, float v15)				__NE___;
	};



	//
	// 512 bit half (AVX512_FP16 + AVX512VL)
	//
	#define AE_SIMD_SimdHalf32
	struct SimdHalf32
	{
	// types
	public:
		using Scalar_t	= half;
		using Self		= SimdHalf32;


	// variables
	private:
		__m512h		_value;		// half[32]


	// methods
	public:
		SimdHalf32 ()	__NE___	: _value{_mm512_setzero_ph()}	{}
	};

#endif // AE_SIMD_AVX >= 3
//-----------------------------------------------------------------------------


#if AE_SIMD_AVX >= 3

	//
	// 512 bit float (AVX512)
	//
	#define AE_SIMD_SimdFloat16
	struct SimdFloat16
	{
	// types
	public:
		using Scalar_t	= float;
		using Self		= SimdFloat16;


	// variables
	private:
		__m512		_value;		// float[16]


	// methods
	public:
		SimdFloat16 ()	__NE___	: _value{_mm512_setzero_ps()}	{}
	};



	//
	// 512 bit double (AVX512)
	//
	#define AE_SIMD_SimdDouble8
	struct SimdDouble8
	{
	// types
	public:
		using Scalar_t	= double;
		using Self		= SimdDouble8;


	// variables
	private:
		__m512d		_value;		// doubel[8]


	// methods
	public:
		SimdDouble8 ()	__NE___	: _value{_mm512_setzero_pd()}	{}
	};



	//
	// 512 bit integer (AVX512)
	//
	#define AE_SIMD_Int512b
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
		Int512b ()		__NE___	: _value{ _mm512_setzero_si512() } {}
	};



	//
	// 512 bit integer (AVX512)
	//
	#define AE_SIMD_SimdTInt512
	template <typename IntType>
	struct SimdTInt512
	{
		StaticAssert(( IsSame<IntType, Int128b> or IsSame<IntType, Int256b> or IsInteger<IntType> ));

	// types
	public:
		using Scalar_t	= IntType;
		using Self		= SimdTInt512< IntType >;


	// variables
	private:
		__m512i		_value;		// long[8], int[16], short[32], byte[64]


	// methods
	public:
		SimdTInt512 ()	__NE___	: _value{_mm512_setzero_si512() } {}

		template <typename IT>
		ND_ SimdTInt512<IT> &		BitCast ()		__NE___	{ return reinterpret_cast< SimdTInt512<IT> &>(*this); }

		template <typename IT>
		ND_ SimdTInt512<IT> const&	BitCast ()		C_NE___	{ return reinterpret_cast< SimdTInt512<IT> const &>(*this); }

		ND_ Int512b const&			BitCast ()		C_NE___	{ return reinterpret_cast< Int512b const &>(*this); }
	};

#endif // AE_SIMD_AVX >= 3
//-----------------------------------------------------------------------------

} // AE::Base
