// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	512 bit SIMD
*/

#pragma once

namespace AE::Base
{
#if 0 //AE_SIMD_AVX >= 30

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

#endif // ???
//-----------------------------------------------------------------------------


#if AE_SIMD_AVX >= 30  // AVX512F


	//
	// 512 bit float (AVX512)
	//
	#define AE_SIMD_SimdFloat16
	struct SimdFloat16
	{
	// types
	public:
		static constexpr uint	count	= 16;
		static constexpr uint	lanes	= 4;

		using Scalar_t		= float;
		using Self			= SimdFloat16;
		using Native_t		= __m512;
		using Array_t		= StaticArray< Scalar_t, count >;
		using Ptr_t			= AlignedPtr< sizeof(Native_t) >;
		using SimdInt_t		= SimdInt16;
		using SimdUInt_t	= SimdUInt16;
		using Mask_t		= Bitfield< __mmask16 >;
		
		StaticAssert( sizeof(Array_t) == sizeof(Native_t) );
		StaticAssert( Mask_t::Count() == count );


		struct Bool16
		{
		private:
			__mmask16	_value;
			
			explicit Bool16 (int val)							__NE___ : _value{__mmask16(val)} {}
		public:
			explicit Bool16 (bool val)							__NE___	: _value{ val ? __mmask16{UMax} : __mmask16{0} } {}
			explicit Bool16 (__mmask16 val)						__NE___ : _value{val} {}
		//	explicit Bool16 (const Native_t &val)				__NE___	: _value{ val } {}
		//	explicit Bool16 (const __m512i &val)				__NE___	: _value{ _mm512_castsi512_ps( val )} {}
		//	explicit Bool16 (const SimdInt_t &v)				__NE___;
		//	explicit Bool16 (const SimdUInt_t &v)				__NE___;
			Bool16 (bool v0, bool v1, bool v2, bool v3,
				    bool v4, bool v5, bool v6, bool v7,
					bool v8, bool v9, bool v10, bool v11,
					bool v12, bool v13, bool v14, bool v15)		__NE___;

			ND_ Bool16	operator | (const Bool16 &rhs)			C_NE___	{ return Bool16{ _value | rhs._value }; }
			ND_ Bool16	operator & (const Bool16 &rhs)			C_NE___	{ return Bool16{ _value & rhs._value }; }
			ND_ Bool16	operator ^ (const Bool16 &rhs)			C_NE___	{ return Bool16{ _value ^ rhs._value }; }
			ND_ Bool16	operator ! ()							C_NE___	{ return Bool16{ ~_value }; }

		//	ND_ Bool16	operator == (const Bool16 &rhs)			C_NE___	{ return Bool16{ _value ? rhs._value }; }
		//	ND_ Bool16	operator != (const Bool16 &rhs)			C_NE___	{ return Bool16{ _value ? rhs._value }; }

			ND_ Bool16	AndNot (const Bool16 &rhs)				C_NE___	{ return Bool16{ ~_value & rhs._value }; }		// ~a & b
		//	ND_ Self	AndNot (const Self &rhs)				C_NE___	{ return Self{ _value, rhs.Ref() )}; }			// ~a & b

		//	ND_ bool	All ()									C_NE___	{ return _mm512_movemask_ps( _value ) == 0xFF; }
		//	ND_ bool	Any ()									C_NE___	{ return _mm512_movemask_ps( _value ) != 0; }
		//	ND_ bool	None ()									C_NE___	{ return _mm512_movemask_ps( _value ) == 0; }

		//	ND_ auto&	Ref ()									C_NE___	{ return _value; }

			ND_ Mask_t	ToBitfield ()							C_NE___	{ return Mask_t{_value}; }

			template <typename DstType>
			ND_ DstType	BitCast ()								C_NE___;

			ND_ static Bool16	True ()							__NE___	{ return Bool16{true}; }
			ND_ static Bool16	False ()						__NE___	{ return Bool16{false}; }
		};
		using Bool_t = Bool16;


	// variables
	private:
		__m512		_value;		// float[16]


	// methods
	public:
		SimdFloat16 ()											__NE___	: _value{ _mm512_setzero_ps() } {}
		SimdFloat16 (Zero_t)									__NE___	: _value{ _mm512_setzero_ps() } {}
		explicit SimdFloat16 (Scalar_t v)						__NE___	: _value{ _mm512_set1_ps( v )} {}
		explicit SimdFloat16 (const Native_t &v)				__NE___	: _value{ v } {}
		explicit SimdFloat16 (const Scalar_t* ptr)				__NE___	: _value{ _mm512_loadu_ps( GetNonNull( ptr ))} {}
		explicit SimdFloat16 (const Ptr_t ptr)					__NE___	: _value{ _mm512_load_ps( ptr.Cast<Scalar_t>() )} {}
		
		explicit SimdFloat16 (const SimdFloat4 &quarter)		__NE___;
		explicit SimdFloat16 (const SimdFloat8 &half)			__NE___;
		SimdFloat16 (const SimdFloat4 &q0, const SimdFloat4 &q1,
					 const SimdFloat4 &q2, const SimdFloat4 &q3) __NE___;
		SimdFloat16 (const SimdFloat8 &h0, const SimdFloat8 &h1) __NE___;

		SimdFloat16 (Scalar_t v0, Scalar_t v1, Scalar_t v2, Scalar_t v3,
					 Scalar_t v4, Scalar_t v5, Scalar_t v6, Scalar_t v7,
					 Scalar_t v8, Scalar_t v9, Scalar_t v10, Scalar_t v11,
					 Scalar_t v12, Scalar_t v13, Scalar_t v14, Scalar_t v15) __NE___ :
			_value{ _mm512_set_ps( v15, v14, v13, v12, v11, v10, v9, v8, v7, v6, v5, v4, v3, v2, v1, v0 )} {}
			
		ND_ Self	operator +  (const Self &rhs)				C_NE___	{ return Add( rhs ); }
		ND_ Self	operator -  (const Self &rhs)				C_NE___	{ return Sub( rhs ); }
		ND_ Self	operator *  (const Self &rhs)				C_NE___	{ return Mul( rhs ); }
		ND_ Self	operator /  (const Self &rhs)				C_NE___	{ return PreciseDiv( rhs ); }

		ND_ Self	operator +  (Scalar_t rhs)					C_NE___	{ return Add( rhs ); }
		ND_ Self	operator -  (Scalar_t rhs)					C_NE___	{ return Sub( rhs ); }
		ND_ Self	operator *  (Scalar_t rhs)					C_NE___	{ return Mul( rhs ); }
		ND_ Self	operator /  (Scalar_t rhs)					C_NE___	{ return PreciseDiv( rhs ); }

		ND_ Bool16	operator == (const Self &rhs)				C_NE___	{ return Equal( rhs ); }
		ND_ Bool16	operator != (const Self &rhs)				C_NE___	{ return NotEqual( rhs ); }
		ND_ Bool16	operator >  (const Self &rhs)				C_NE___	{ return Greater( rhs ); }
		ND_ Bool16	operator <  (const Self &rhs)				C_NE___	{ return Less( rhs ); }
		ND_ Bool16	operator >= (const Self &rhs)				C_NE___	{ return GEqual( rhs ); }
		ND_ Bool16	operator <= (const Self &rhs)				C_NE___	{ return LEqual( rhs ); }
		
		template <uint I> ND_ Scalar_t	get ()					C_NE___;
		template <uint I> ND_ Self		set (Scalar_t val)		C_NE___;

		ND_ Native_t &		Ref ()								__NE___	{ return _value; }
		ND_ Native_t const&	Ref ()								C_NE___	{ return _value; }
		
		ND_ Self	Add (const Self &rhs)						C_NE___	{ return Self{ _mm512_add_ps( _value, rhs._value )}; }
		ND_ Self	Sub (const Self &rhs)						C_NE___	{ return Self{ _mm512_sub_ps( _value, rhs._value )}; }
		ND_ Self	Mul (const Self &rhs)						C_NE___	{ return Self{ _mm512_mul_ps( _value, rhs._value )}; }
		ND_ Self	PreciseDiv (const Self &rhs)				C_NE___	{ return Self{ _mm512_div_ps( _value, rhs._value )}; }
		ND_ Self	Min (const Self &rhs)						C_NE___	{ return Self{ _mm512_min_ps( _value, rhs._value )}; }
		ND_ Self	Max (const Self &rhs)						C_NE___	{ return Self{ _mm512_max_ps( _value, rhs._value )}; }

		ND_ Self	Add (Scalar_t rhs)							C_NE___	{ return Self{ _mm512_add_ps( _value, _mm512_set1_ps(rhs) )}; }
		ND_ Self	Sub (Scalar_t rhs)							C_NE___	{ return Self{ _mm512_sub_ps( _value, _mm512_set1_ps(rhs) )}; }
		ND_ Self	Mul (Scalar_t rhs)							C_NE___	{ return Self{ _mm512_mul_ps( _value, _mm512_set1_ps(rhs) )}; }
		ND_ Self	PreciseDiv (Scalar_t rhs)					C_NE___	{ return Self{ _mm512_div_ps( _value, _mm512_set1_ps(rhs) )}; }
		ND_ Self	Min (Scalar_t rhs)							C_NE___	{ return Self{ _mm512_min_ps( _value, _mm512_set1_ps(rhs) )}; }
		ND_ Self	Max (Scalar_t rhs)							C_NE___	{ return Self{ _mm512_max_ps( _value, _mm512_set1_ps(rhs) )}; }

		ND_ Self	Abs ()										C_NE___	{ return Self{ _mm512_andnot_ps( _mm512_set1_ps( -0.0f ), _value )}; }
		ND_ Self	PreciseSqrt ()								C_NE___	{ return Self{ _mm512_sqrt_ps( _value )}; }

		ND_ Self	Reciprocal ()								C_NE___	{ return Self{ _mm512_rcp14_ps( _value )}; }								// approx (1 / x)
		ND_ Self	FastInvSqrt ()								C_NE___	{ return Self{ _mm512_rsqrt14_ps( _value )}; }								// approx (1 / sqrt(x))
		ND_ Self	FastSqrt ()									C_NE___	{ return Self{ _mm512_mul_ps( _value, _mm512_rsqrt14_ps( _value ))}; }		// approx (x / sqrt(x))
		ND_ Self	FastDiv (const Self &rhs)					C_NE___	{ return Self{ _mm512_mul_ps( _value, _mm512_rcp14_ps( rhs._value ))}; }	// approx (a / b)
		
		// ordered - comparison with NaN returns false
		ND_ Bool16	Equal    (const Self &rhs)					C_NE___	{ return Bool16{ _mm512_cmp_ps_mask( _value, rhs._value, _CMP_EQ_OQ  )}; }
		ND_ Bool16	NotEqual (const Self &rhs)					C_NE___	{ return Bool16{ _mm512_cmp_ps_mask( _value, rhs._value, _CMP_NEQ_OQ )}; }
		ND_ Bool16	Greater  (const Self &rhs)					C_NE___	{ return Bool16{ _mm512_cmp_ps_mask( _value, rhs._value, _CMP_GT_OQ  )}; }
		ND_ Bool16	Less     (const Self &rhs)					C_NE___	{ return Bool16{ _mm512_cmp_ps_mask( _value, rhs._value, _CMP_LT_OQ  )}; }
		ND_ Bool16	GEqual   (const Self &rhs)					C_NE___	{ return Bool16{ _mm512_cmp_ps_mask( _value, rhs._value, _CMP_GE_OQ  )}; }
		ND_ Bool16	LEqual   (const Self &rhs)					C_NE___	{ return Bool16{ _mm512_cmp_ps_mask( _value, rhs._value, _CMP_LE_OQ  )}; }


	};



	//
	// 512 bit double (AVX512)
	//
	#define AE_SIMD_SimdDouble8
	struct SimdDouble8
	{
	// types
	public:
		static constexpr uint	count	= 8;
		static constexpr uint	lanes	= 4;

		using Scalar_t		= double;
		using Self			= SimdDouble8;
		using Native_t		= __m512d;
		using Array_t		= StaticArray< Scalar_t, count >;
		using Ptr_t			= AlignedPtr< sizeof(Native_t) >;
		using SimdInt_t		= SimdLong8;
		using SimdUInt_t	= SimdULong8;
		using Mask_t		= Bitfield< __mmask8 >;

		StaticAssert( sizeof(Array_t) == sizeof(Native_t) );
		StaticAssert( Mask_t::Count() == count );


		struct Bool8
		{
		private:
			__mmask8	_value;
			
			explicit Bool8 (int val)							__NE___ : _value{__mmask8(val)} {}
		public:
			explicit Bool8 (bool val)							__NE___	: _value{ val ? __mmask8{UMax} : __mmask8{0} } {}
			explicit Bool8 (__mmask8 val)						__NE___ : _value{val} {}
			Bool8 (bool v0, bool v1, bool v2, bool v3,
				   bool v4, bool v5, bool v6, bool v7)			__NE___;
		};
		using Bool_t = Bool8;


	// variables
	private:
		__m512d		_value;		// doubel[8]


	// methods
	public:
		SimdDouble8 ()													__NE___	: _value{ _mm512_setzero_pd() } {}
		SimdDouble8 (Zero_t)											__NE___	: _value{ _mm512_setzero_pd() } {}
		explicit SimdDouble8 (Scalar_t v)								__NE___	: _value{ _mm512_set1_pd( v )} {}
		explicit SimdDouble8 (const Native_t &v)						__NE___	: _value{ v } {}
		explicit SimdDouble8 (const Scalar_t* ptr)						__NE___	: _value{ _mm512_loadu_pd( GetNonNull( ptr ))} {}
		explicit SimdDouble8 (const Ptr_t ptr)							__NE___	: _value{ _mm512_load_pd( ptr.Cast<Scalar_t>() )} {}
		SimdDouble8 (Scalar_t v0, Scalar_t v1, Scalar_t v2, Scalar_t v3,
					 Scalar_t v4, Scalar_t v5, Scalar_t v6, Scalar_t v7) __NE___ : _value{ _mm512_set_pd( v7, v6, v5, v4, v3, v2, v1, v0 )} {}

		explicit SimdDouble8 (const SimdDouble2 &quart)					__NE___;
		explicit SimdDouble8 (const SimdDouble4 &half)					__NE___;
		SimdDouble8 (const SimdDouble2 &q0, const SimdDouble2 &q1,
					 const SimdDouble2 &q2, const SimdDouble2 &q3)		__NE___;

		ND_ Self	operator +  (const Self &rhs)				C_NE___	{ return Add( rhs ); }
		ND_ Self	operator -  (const Self &rhs)				C_NE___	{ return Sub( rhs ); }
		ND_ Self	operator *  (const Self &rhs)				C_NE___	{ return Mul( rhs ); }
		ND_ Self	operator /  (const Self &rhs)				C_NE___	{ return PreciseDiv( rhs ); }

		ND_ Self	operator +  (Scalar_t rhs)					C_NE___	{ return Add( rhs ); }
		ND_ Self	operator -  (Scalar_t rhs)					C_NE___	{ return Sub( rhs ); }
		ND_ Self	operator *  (Scalar_t rhs)					C_NE___	{ return Mul( rhs ); }
		ND_ Self	operator /  (Scalar_t rhs)					C_NE___	{ return PreciseDiv( rhs ); }

		ND_ Bool8	operator == (const Self &rhs)				C_NE___	{ return Equal( rhs ); }
		ND_ Bool8	operator != (const Self &rhs)				C_NE___	{ return NotEqual( rhs ); }
		ND_ Bool8	operator >  (const Self &rhs)				C_NE___	{ return Greater( rhs ); }
		ND_ Bool8	operator <  (const Self &rhs)				C_NE___	{ return Less( rhs ); }
		ND_ Bool8	operator >= (const Self &rhs)				C_NE___	{ return GEqual( rhs ); }
		ND_ Bool8	operator <= (const Self &rhs)				C_NE___	{ return LEqual( rhs ); }

		template <uint I> ND_ Scalar_t	get ()					C_NE___;
		template <uint I> ND_ Self		set (Scalar_t val)		C_NE___;

		ND_ Native_t &		Ref ()								__NE___	{ return _value; }
		ND_ Native_t const&	Ref ()								C_NE___	{ return _value; }

		ND_ Self	Add (const Self &rhs)						C_NE___	{ return Self{ _mm512_add_pd( _value, rhs._value )}; }
		ND_ Self	Sub (const Self &rhs)						C_NE___	{ return Self{ _mm512_sub_pd( _value, rhs._value )}; }
		ND_ Self	Mul (const Self &rhs)						C_NE___	{ return Self{ _mm512_mul_pd( _value, rhs._value )}; }
		ND_ Self	PreciseDiv (const Self &rhs)				C_NE___	{ return Self{ _mm512_div_pd( _value, rhs._value )}; }
		ND_ Self	Min (const Self &rhs)						C_NE___	{ return Self{ _mm512_min_pd( _value, rhs._value )}; }
		ND_ Self	Max (const Self &rhs)						C_NE___	{ return Self{ _mm512_max_pd( _value, rhs._value )}; }

		ND_ Self	Add (Scalar_t rhs)							C_NE___	{ return Self{ _mm512_add_pd( _value, _mm512_set1_pd(rhs) )}; }
		ND_ Self	Sub (Scalar_t rhs)							C_NE___	{ return Self{ _mm512_sub_pd( _value, _mm512_set1_pd(rhs) )}; }
		ND_ Self	Mul (Scalar_t rhs)							C_NE___	{ return Self{ _mm512_mul_pd( _value, _mm512_set1_pd(rhs) )}; }
		ND_ Self	PreciseDiv (Scalar_t rhs)					C_NE___	{ return Self{ _mm512_div_pd( _value, _mm512_set1_pd(rhs) )}; }
		ND_ Self	Min (Scalar_t rhs)							C_NE___	{ return Self{ _mm512_min_pd( _value, _mm512_set1_pd(rhs) )}; }
		ND_ Self	Max (Scalar_t rhs)							C_NE___	{ return Self{ _mm512_max_pd( _value, _mm512_set1_pd(rhs) )}; }

		ND_ Self	Abs ()										C_NE___	{ return Self{ _mm512_andnot_pd( _mm512_set1_pd( -0.0 ), _value )}; }
		ND_ Self	PreciseSqrt ()								C_NE___	{ return Self{ _mm512_sqrt_pd( _value )}; }
		
		ND_ Self	Reciprocal ()								C_NE___	{ return Self{ _mm512_rcp14_pd( _value )}; }			// approx (1 / x)
		ND_ Self	FastDiv (const Self &rhs)					C_NE___	{ return *this * rhs.Reciprocal(); }					// approx (a / b)
		
		// ordered - comparison with NaN returns false
		ND_ Bool8	Equal    (const Self &rhs)					C_NE___	{ return Bool8{ _mm512_cmp_pd_mask( _value, rhs._value, _CMP_EQ_OQ  )}; }
		ND_ Bool8	NotEqual (const Self &rhs)					C_NE___	{ return Bool8{ _mm512_cmp_pd_mask( _value, rhs._value, _CMP_NEQ_OQ )}; }
		ND_ Bool8	Greater  (const Self &rhs)					C_NE___	{ return Bool8{ _mm512_cmp_pd_mask( _value, rhs._value, _CMP_GT_OQ  )}; }
		ND_ Bool8	Less     (const Self &rhs)					C_NE___	{ return Bool8{ _mm512_cmp_pd_mask( _value, rhs._value, _CMP_LT_OQ  )}; }
		ND_ Bool8	GEqual   (const Self &rhs)					C_NE___	{ return Bool8{ _mm512_cmp_pd_mask( _value, rhs._value, _CMP_GE_OQ  )}; }
		ND_ Bool8	LEqual   (const Self &rhs)					C_NE___	{ return Bool8{ _mm512_cmp_pd_mask( _value, rhs._value, _CMP_LE_OQ  )}; }
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


#endif // AE_SIMD_AVX >= 30
//-----------------------------------------------------------------------------

} // AE::Base
