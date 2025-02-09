// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
 Neon:
	https://developer.arm.com/architectures/instruction-sets/intrinsics/
	https://arm-software.github.io/acle/neon_intrinsics/advsimd.html
	https://docs.unity3d.com/Packages/com.unity.burst@1.8/manual/csharp-burst-intrinsics-neon.html

 Pending features:
	__ARM_FEATURE_BF16
	__ARM_FEATURE_SVE
	__ARM_FEATURE_SVE_BF16
	__ARM_FEATURE_SVE2_AES
	__ARM_FEATURE_FP16_FML

TODO:
	intrinsics: asimd, aes, pmull sha1 sha2 crc32, fphp, asimdhp, asimdrdm, asimddp

	equal to zero, Pairwise arithmetic, zip, uzp, ext, rev, mov_n
*/

#pragma once

#if AE_SIMD_NEON

# ifdef AE_CPU_ARCH_ARM64
#	define AE_SIMD_NEON64	1
# else
#	define AE_SIMD_NEON64	0
# endif

namespace AE::Base
{
# if AE_SIMD_NEON_HALF

	//
	// 64 bit half float (Neon)
	//
	#define AE_SIMD_SimdHalf4
	struct SimdHalf4
	{
		friend struct SimdHalf8;

	// types
	public:
		static constexpr uint	count	= 4;
		static constexpr uint	lanes	= 1;

		using Scalar_t		= half;
		using NativeScalar_t= float16_t;
		using Self			= SimdHalf4;
		using Native_t		= float16x4_t;
		using Array_t		= StaticArray< Scalar_t, count >;
		using SimdInt_t		= SimdShort4;
		using SimdUInt_t	= SimdUShort4;
		using Mask_t		= Base::_hidden_::MSBMask< count >;

		StaticAssert( sizeof(Array_t) == sizeof(Native_t) );


		struct Bool4
		{
		private:
			uint16x4_t	_value;

		public:
			explicit Bool4 (bool val)							__NE___;
			explicit Bool4 (const uint16x4_t &v)				__NE___	: _value{v} {}
			explicit Bool4 (const SimdInt_t &v)					__NE___;
			explicit Bool4 (const SimdUInt_t &v)				__NE___;
			Bool4 (bool v0, bool v1, bool v2, bool v3)			__NE___;

			ND_ Bool4	operator | (const Bool4 &rhs)			C_NE___	{ return Bool4{ vorr_u16( _value, rhs._value )}; }
			ND_ Bool4	operator & (const Bool4 &rhs)			C_NE___	{ return Bool4{ vand_u16( _value, rhs._value )}; }
			ND_ Bool4	operator ^ (const Bool4 &rhs)			C_NE___	{ return Bool4{ veor_u16( _value, rhs._value )}; }
			ND_ Bool4	operator ! ()							C_NE___	{ return Bool4{ vmvn_u16( _value )}; }

			ND_ Bool4	operator == (const Bool4 &rhs)			C_NE___	{ return Bool4{ vand_u16( _value, rhs._value )}; }
			ND_ Bool4	operator != (const Bool4 &rhs)			C_NE___	{ return Bool4{ vbic_u16( rhs._value, _value )}; }

			ND_ Bool4	OrNot (const Bool4 &rhs)				C_NE___	{ return Bool4{ vorn_u16( _value, rhs._value )}; }		// a | ~b
			ND_ Self	OrNot (const Self &rhs)					C_NE___	{ return Self{ vorn_u16( _value, rhs.Ref() )}; }		// a | ~b

			ND_ Bool4	AndNot (const Bool4 &rhs)				C_NE___	{ return Bool4{ vbic_u16( _value, rhs._value )}; }		// ~a & b
			ND_ Self	AndNot (const Self &rhs)				C_NE___	{ return Self{ vbic_u16( _value, rhs.Ref() )}; }		// ~a & b

			template <uint I> ND_ bool	 get ()					C_NE___	{ StaticAssert( I < count );  return vget_lane_u16( _value, I ) == UMax; }
			//template <uint I> ND_ Bool4  set (bool val)		C_NE___	{ StaticAssert( I < count );  return Bool4{ vset_lane_u16( (val ? UMax : Zero), _value, I )}; }

			ND_ bool	All ()									C_NE___;
			ND_ bool	Any ()									C_NE___;
			ND_ bool	None ()									C_NE___;

			ND_ auto&	Ref ()									C_NE___	{ return _value; }

			ND_ Mask_t	ToBitfield ()							C_NE___;

			template <typename DstType>
			ND_ DstType	BitCast ()								C_NE___;

			ND_ static Bool4	True ()							__NE___	{ return Bool4{ vdup_n_u16( UMax )}; }
			ND_ static Bool4	False ()						__NE___	{ return Bool4{ vdup_n_u16( 0 )}; }
		};
		using Bool_t = Bool4;


	// variables
	private:
		Native_t	_value;		// half[4]

		static constexpr ushort		c_One_BitMask = 0x3c00;	// bitcast<ushort>( 1.hf )


	// methods
	public:
		SimdHalf4 ()											__NE___	: SimdHalf4{ float16_t{0} } {}
		SimdHalf4 (Zero_t)										__NE___ : SimdHalf4{ float16_t{0} } {}
		explicit SimdHalf4 (Scalar_t val)						__NE___	: SimdHalf4{ Base::BitCast<float16_t>( val )} {}
		explicit SimdHalf4 (float16_t val)						__NE___	: _value{ vdup_n_f16( val )} {}
		explicit SimdHalf4 (const float16_t* ptr)				__NE___	: _value{ vld1_f16( GetNonNull( ptr ))} {}
		explicit SimdHalf4 (const Scalar_t* ptr)				__NE___	: SimdHalf4{ Base::Cast<float16_t>( ptr )} {}
		explicit SimdHalf4 (const Native_t &val)				__NE___	: _value{ val } {}
		explicit SimdHalf4 (const Bool4 &v)						__NE___ : _value{ vreinterpret_f16_u16( vand_u16( v.Ref(), vdup_n_u16(c_One_BitMask) ))} {}
		SimdHalf4 (Scalar_t x, Scalar_t y, Scalar_t z, Scalar_t w)		__NE___;
		SimdHalf4 (float16_t x, float16_t y, float16_t z, float16_t w)	__NE___;

		ND_ Self	operator - ()								C_NE___	{ return Negative(); }

		ND_ Self	operator +  (const Self &rhs)				C_NE___	{ return Add( rhs ); }
		ND_ Self	operator -  (const Self &rhs)				C_NE___	{ return Sub( rhs ); }
		ND_ Self	operator *  (const Self &rhs)				C_NE___	{ return Mul( rhs ); }

		ND_ Self	operator +  (float16_t rhs)					C_NE___	{ return Add( rhs ); }
		ND_ Self	operator -  (float16_t rhs)					C_NE___	{ return Sub( rhs ); }
		ND_ Self	operator *  (float16_t rhs)					C_NE___	{ return Mul( rhs ); }
		ND_ Self	operator /  (float16_t rhs)					C_NE___	{ return PreciseDiv( rhs ); }

		ND_ Self	operator +  (Scalar_t rhs)					C_NE___	{ return Add( rhs ); }
		ND_ Self	operator -  (Scalar_t rhs)					C_NE___	{ return Sub( rhs ); }
		ND_ Self	operator *  (Scalar_t rhs)					C_NE___	{ return Mul( rhs ); }
		ND_ Self	operator /  (Scalar_t rhs)					C_NE___	{ return PreciseDiv( rhs ); }

		ND_ Bool4	operator == (const Self &rhs)				C_NE___	{ return Equal( rhs ); }
		ND_ Bool4	operator != (const Self &rhs)				C_NE___	{ return NotEqual( rhs ); }
		ND_ Bool4	operator >  (const Self &rhs)				C_NE___	{ return Greater( rhs ); }
		ND_ Bool4	operator <  (const Self &rhs)				C_NE___	{ return Less( rhs ); }
		ND_ Bool4	operator >= (const Self &rhs)				C_NE___	{ return GEqual( rhs ); }
		ND_ Bool4	operator <= (const Self &rhs)				C_NE___	{ return LEqual( rhs ); }

		template <uint I> ND_ Scalar_t	get ()					C_NE___	{ StaticAssert( I < count );  return Scalar_t{vget_lane_f16( _value, I )}; }
		template <uint I> ND_ Self		set (float16_t val)		C_NE___	{ StaticAssert( I < count );  return Self{ vset_lane_f16( val, _value, I )}; }
		template <uint I> ND_ Self		set (Scalar_t val)		C_NE___	{ return set<I>( float16_t{val} ); }

		ND_ Native_t&		Ref ()								__NE___	{ return _value; }
		ND_ Native_t const&	Ref ()								C_NE___	{ return _value; }

	  #if AE_SIMD_NEON64
		ND_ Scalar_t	PrefixMax ()							C_NE___	{ return Scalar_t{ vmaxv_f16( _value )}; }
		ND_ Scalar_t	PrefixMin ()							C_NE___	{ return Scalar_t{ vminv_f16( _value )}; }
	  #endif

		ND_ Self	Add (const Self &rhs)						C_NE___	{ return Self{ vadd_f16( _value, rhs._value )}; }
		ND_ Self	Sub (const Self &rhs)						C_NE___	{ return Self{ vsub_f16( _value, rhs._value )}; }
		ND_ Self	Mul (const Self &rhs)						C_NE___	{ return Self{ vmul_f16( _value, rhs._value )}; }
		ND_ Self	Min (const Self &rhs)						C_NE___	{ return Self{ vmin_f16( _value, rhs._value )}; }
		ND_ Self	Max (const Self &rhs)						C_NE___	{ return Self{ vmax_f16( _value, rhs._value )}; }

		ND_ Self	Add (float16_t rhs)							C_NE___	{ return Self{ vadd_f16( _value, vdup_n_f16(rhs) )}; }
		ND_ Self	Sub (float16_t rhs)							C_NE___	{ return Self{ vsub_f16( _value, vdup_n_f16(rhs) )}; }
		ND_ Self	Mul (float16_t rhs)							C_NE___	{ return Self{ vmul_n_f16( _value, rhs )}; }
		ND_ Self	Min (float16_t rhs)							C_NE___	{ return Self{ vmin_f16( _value, vdup_n_f16(rhs) )}; }
		ND_ Self	Max (float16_t rhs)							C_NE___	{ return Self{ vmax_f16( _value, vdup_n_f16(rhs) )}; }
		ND_ Self	PreciseDiv (float16_t rhs)					C_NE___	{ return Mul( float16_t( 1.0f / float(rhs) )); }

		ND_ Self	Add (Scalar_t rhs)							C_NE___	{ return Add( float16_t{rhs} ); }
		ND_ Self	Sub (Scalar_t rhs)							C_NE___	{ return Sub( float16_t{rhs} ); }
		ND_ Self	Mul (Scalar_t rhs)							C_NE___	{ return Mul( float16_t{rhs} ); }
		ND_ Self	Min (Scalar_t rhs)							C_NE___	{ return Min( float16_t{rhs} ); }
		ND_ Self	Max (Scalar_t rhs)							C_NE___	{ return Max( float16_t{rhs} ); }
		ND_ Self	PreciseDiv (Scalar_t rhs)					C_NE___	{ return Mul( float16_t( 1.0f / float(rhs) )); }

		ND_ Bool4	Equal    (const Self &rhs)					C_NE___	{ return Bool4{ vceq_f16( _value, rhs._value )}; }
		ND_ Bool4	NotEqual (const Self &rhs)					C_NE___	{ return not Equal( rhs ); }
		ND_ Bool4  	Greater  (const Self &rhs)					C_NE___	{ return Bool4{ vcgt_f16( _value, rhs._value )}; }
		ND_ Bool4  	GEqual   (const Self &rhs)					C_NE___	{ return Bool4{ vcge_f16( _value, rhs._value )}; }
		ND_ Bool4  	Less     (const Self &rhs)					C_NE___	{ return Bool4{ vclt_f16( _value, rhs._value )}; }
		ND_ Bool4  	LEqual   (const Self &rhs)					C_NE___	{ return Bool4{ vcle_f16( _value, rhs._value )}; }

		ND_ Bool4  	AbsGreater (const Self &rhs)				C_NE___	{ return Bool4{ vcage_f16( _value, rhs._value )}; }
		ND_ Bool4  	AbsGEqual  (const Self &rhs)				C_NE___	{ return Bool4{ vcage_f16( _value, rhs._value )}; }
		ND_ Bool4  	AbsLess    (const Self &rhs)				C_NE___	{ return Bool4{ vcalt_f16( _value, rhs._value )}; }
		ND_ Bool4  	AbsLEqual  (const Self &rhs)				C_NE___	{ return Bool4{ vcale_f16( _value, rhs._value )}; }

		// compare and return 0.0 or 1.0
		ND_ Self  	EqualF    (const Self &rhs)					C_NE___	{ return Self{ Equal( rhs )}; }
		ND_ Self  	NotEqualF (const Self &rhs)					C_NE___	{ return Self{ NotEqual( rhs )}; }
		ND_ Self  	GreaterF  (const Self &rhs)					C_NE___	{ return Self{ Greater( rhs )}; }
		ND_ Self  	GEqualF   (const Self &rhs)					C_NE___	{ return Self{ GEqual( rhs )}; }
		ND_ Self  	LessF     (const Self &rhs)					C_NE___	{ return Self{ Less( rhs )}; }
		ND_ Self  	LEqualF   (const Self &rhs)					C_NE___	{ return Self{ LEqual( rhs )}; }

		ND_ Self  	AbsGreaterF (const Self &rhs)				C_NE___	{ return Self{ AbsGreater( rhs )}; }
		ND_ Self  	AbsGEqualF  (const Self &rhs)				C_NE___	{ return Self{ AbsGEqual( rhs )}; }
		ND_ Self  	AbsLessF    (const Self &rhs)				C_NE___	{ return Self{ AbsLess( rhs )}; }
		ND_ Self  	AbsLEqualF  (const Self &rhs)				C_NE___	{ return Self{ AbsLEqual( rhs )}; }

		ND_ Bool4	BitEqual (const Self&, EnabledBitCount acc)	C_NE___;

		ND_ Self	Abs ()										C_NE___	{ return Self{ vabs_f16( _value )}; }							// abs(x)
		ND_ Self	Negative ()									C_NE___	{ return Self{ vneg_f16( _value )}; }							// -x

		ND_ Self	Reciprocal ()								C_NE___	{ return Self{ vrecpe_f16( _value )}; }							// approx (1 / x)
		ND_ Self	FastInvSqrt ()								C_NE___	{ return Self{ vrsqrte_f16( _value )}; }						// approx (1 / sqrt(x))
		ND_ Self	FastSqrt ()									C_NE___	{ return Self{ vmul_f16( _value, vrsqrte_f16( _value ))}; }		// approx (x / sqrt(x))
		ND_ Self	FastDiv (const Self &rhs)					C_NE___	{ return Self{ vmul_f16( _value, vrecpe_f16( rhs._value ))}; }	// approx (a / b)

	  #if AE_SIMD_NEON64
		ND_ Self	MulExt (const Self &rhs)					C_NE___	{ return Self{ vmulx_f16( _value, rhs._value )}; }
		ND_ Self	MulExt (float16_t rhs)						C_NE___	{ return Self{ vmulx_n_f16( _value, rhs )}; }

		ND_ Self	PreciseSqrt ()								C_NE___	{ return Self{ vsqrt_f16( _value )}; }

		ND_ Self	PreciseDiv (const Self &rhs)				C_NE___	{ return Self{ vdiv_f16( _value, rhs._value )}; }
		ND_ Self	operator / (const Self &rhs)				C_NE___	{ return PreciseDiv( rhs ); }
	  #else
		ND_ Self	operator / (const Self &rhs)				C_NE___	{ return FastDiv( rhs ); }
	  #endif

	  #ifdef AE_PLATFORM_APPLE
		ND_ Self	Trunc ()									C_NE___	{ return Self{ vrnd_f16(  _value )}; }
		ND_ Self	Floor ()									C_NE___	{ return Self{ vrndm_f16( _value )}; }
		ND_ Self	Ceil ()										C_NE___	{ return Self{ vrndp_f16( _value )}; }
		ND_ Self	Round ()									C_NE___	{ return Self{ vrnda_f16( _value )}; }	// or vrndi_f16, vrndx_f16
		ND_ Self	RoundEven ()								C_NE___	{ return Self{ vrndn_f16( _value )}; }
	  #endif

	  #if AE_SIMD_FMA
		ND_ friend Self  FusedMulAdd    (const Self &a, const Self &b, const Self &c)	__NE___	{ return Self{ vfma_f16(   c._value, a._value, b._value )}; }	// (a * b) + c
		ND_ friend Self  FusedNegMulAdd (const Self &a, const Self &b, const Self &c)	__NE___	{ return Self{ vfms_f16(   c._value, a._value, b._value )}; }	// c - (a * b)
	  #endif
	  #if AE_SIMD_NEON64 and AE_SIMD_FMA
		ND_ friend Self  FusedMulAdd    (const Self &a, float16_t b, const Self &c)		__NE___	{ return Self{ vfma_n_f16( c._value, a._value, b )}; }			// (a * b) + c
		ND_ friend Self  FusedNegMulAdd (const Self &a, float16_t b, const Self &c)		__NE___	{ return Self{ vfms_n_f16( c._value, a._value, b )}; }			// c - (a * b)
	  #endif

		ND_ friend Self  Lerp    (const Self &x, const Self &y, const Self &factor)						__NE___	{ return x * (Self{1.f} - factor) + y * factor; }
		ND_ friend Self  Select  (const Bool4 &condition, const Self &ifTrue, const Self &ifFalse)		__NE___	{ return Self{ vbsl_f16( condition.Ref(), ifTrue._value, ifFalse._value )}; }
		ND_ friend Self  SelectF (const Self &x, const Self &y, const Self &ifTrue, const Self &ifFalse)__NE___	{ return Lerp( ifFalse, ifTrue, x.LessF(y) ); }


	// per bit operations //
		ND_ Self	BitInverse ()								C_NE___	{ return Self{ vreinterpret_f16_u16( vmvn_u16( vreinterpret_u16_f16( _value )))}; }

		ND_ Self	And (const Self &rhs)						C_NE___	{ return Self{ vreinterpret_f16_u16( vand_u16( vreinterpret_u16_f16(_value), vreinterpret_u16_f16(rhs._value) ))}; }
		ND_ Self	Or  (const Self &rhs)						C_NE___	{ return Self{ vreinterpret_f16_u16( vorr_u16( vreinterpret_u16_f16(_value), vreinterpret_u16_f16(rhs._value) ))}; }
		ND_ Self	Xor (const Self &rhs)						C_NE___	{ return Self{ vreinterpret_f16_u16( veor_u16( vreinterpret_u16_f16(_value), vreinterpret_u16_f16(rhs._value) ))}; }
		ND_ Self	OrNot (const Self &rhs)						C_NE___	{ return Self{ vreinterpret_f16_u16( vorn_u16( vreinterpret_u16_f16(_value), vreinterpret_u16_f16(rhs._value) ))}; }	// a | ~b
		ND_ Self	AndNot (const Self &rhs)					C_NE___	{ return Self{ vreinterpret_f16_u16( vbic_u16( vreinterpret_u16_f16(_value), vreinterpret_u16_f16(rhs._value) ))}; }	// ~a & b

		ND_ Self	And (const Bool4 &rhs)						C_NE___	{ return Self{ vreinterpret_f16_u16( vand_u16( vreinterpret_u16_f16(_value), rhs.Ref() ))}; }
		ND_ Self	Or  (const Bool4 &rhs)						C_NE___	{ return Self{ vreinterpret_f16_u16( vorr_u16( vreinterpret_u16_f16(_value), rhs.Ref() ))}; }
		ND_ Self	Xor (const Bool4 &rhs)						C_NE___	{ return Self{ vreinterpret_f16_u16( veor_u16( vreinterpret_u16_f16(_value), rhs.Ref() ))}; }
		ND_ Self	OrNot (const Bool4 &rhs)					C_NE___	{ return Self{ vreinterpret_f16_u16( vorn_u16( vreinterpret_u16_f16(_value), rhs.Ref() ))}; }	// a | ~b
		ND_ Self	AndNot (const Bool4 &rhs)					C_NE___	{ return Self{ vreinterpret_f16_u16( vbic_u16( vreinterpret_u16_f16(_value), rhs.Ref() ))}; }	// ~a & b

		ND_ Self	And (const ushort rhs)						C_NE___	{ return Self{ vreinterpret_f16_u16( vand_u16( vreinterpret_u16_f16(_value), vdup_n_u16(rhs) ))}; }
		ND_ Self	Or  (const ushort rhs)						C_NE___	{ return Self{ vreinterpret_f16_u16( vorr_u16( vreinterpret_u16_f16(_value), vdup_n_u16(rhs) ))}; }
		ND_ Self	Xor (const ushort rhs)						C_NE___	{ return Self{ vreinterpret_f16_u16( veor_u16( vreinterpret_u16_f16(_value), vdup_n_u16(rhs) ))}; }
		ND_ Self	OrNot (const ushort rhs)					C_NE___	{ return Self{ vreinterpret_f16_u16( vorn_u16( vreinterpret_u16_f16(_value), vdup_n_u16(rhs) ))}; }	// a | ~b
		ND_ Self	AndNot (const ushort rhs)					C_NE___	{ return Self{ vreinterpret_f16_u16( vbic_u16( vreinterpret_u16_f16(_value), vdup_n_u16(rhs) ))}; }	// ~a & b


	// conversion //
		template <uint Idx>
		ND_ Self	Lane ()										C_NE___ { StaticAssert( Idx < lanes );  return *this; }

		template <uint X, uint Y, uint Z, uint W>
		ND_ Self	Swizzle ()									C_NE___;

		template <typename DstScalar>
		ND_ auto	Convert ()									C_NE___;

		template <typename DstType>
		ND_ DstType	BitCast ()									C_NE___;

		ND_ SimdShort4	ToShort ()								C_NE___;
		ND_ SimdFloat4	ToFloat ()								C_NE___;

		void		ToArray (OUT float16_t* dst)				C_NE___	{ NonNull(dst);  vst1_f16( OUT dst, _value ); }
		void		ToArray (OUT Scalar_t* dst)					C_NE___	{ ToArray( Base::Cast<float16_t>(dst) ); }
		ND_ Array_t	ToArray ()									C_NE___	{ Array_t arr;  ToArray( OUT arr.data() );  return arr; }


	// Features //
		NdCe__ static bool  Has_Arithmetic ()					{ return true; }
		NdCe__ static bool  Has_Equal ()						{ return true; }
		NdCe__ static bool  Has_Greater ()						{ return true; }
	  #ifdef AE_PLATFORM_APPLE
		NdCe__ static bool  Has_Rounding ()						{ return true; }
	  #else
		NdCe__ static bool  Has_Rounding ()						{ return false; }
	  #endif
		NdCe__ static bool  Has_FusedMulAdd ()					{ return AE_SIMD_FMA; }
		NdCe__ static bool  Has_MulAdd ()						{ return false; }
		NdCe__ static bool  Has_PreciseSqrt ()					{ return AE_SIMD_NEON64; }
		NdCe__ static bool  Has_PreciseInvSqrt ()				{ return false; }
		NdCe__ static bool  Has_PreciseDiv ()					{ return AE_SIMD_NEON64; }
		NdCe__ static bool  Has_ApproxReciprocal ()				{ return true; }
		NdCe__ static bool  Has_ApproxInvSqrt ()				{ return true; }
		NdCe__ static bool  Has_Trigonometry ()					{ return false; }
		NdCe__ static bool  Has_Exponential ()					{ return false; }
		NdCe__ static bool  Has_PrefixSum ()					{ return false; }
		NdCe__ static bool  Has_PrefixMinMax ()					{ return AE_SIMD_NEON64; }
		NdCe__ static bool  Has_BitEqual ()						{ return false; }
		NdCe__ static bool  Has_Swizzle ()						{ return true; }
		NdCe__ static bool  Has_Shuffle ()						{ return false; }

		template <typename DstType>	NdCe__ static bool  Has_BitCast ();
		template <typename DstType>	NdCe__ static bool  Has_Convert ();
	};

# endif // AE_SIMD_NEON_HALF
//-----------------------------------------------------------------------------



	//
	// 64 bit integer (Neon)
	//
	#define AE_SIMD_SimdTInt64
	template <typename IntType>
	struct SimdTInt64
	{
		StaticAssert( IsInteger<IntType> );

	// types
	public:
		using Scalar_t	= IntType;
		using Self		= SimdTInt64< IntType >;
		using Bool_t	= SimdTInt64< ToUnsignedInteger< IntType >>;
		using Native_t	= Conditional< IsSame< IntType, sbyte >, int8x8_t,
							Conditional< IsSame< IntType, ubyte >, uint8x8_t,
								Conditional< IsSame< IntType, sshort >, int16x4_t,
									Conditional< IsSame< IntType, ushort >, uint16x4_t,
										Conditional< IsSame< IntType, sint >, int32x2_t,
											Conditional< IsSame< IntType, uint >, uint32x2_t, void >>>>>>;
		StaticAssert( CT_SizeOfInBits<Native_t> == 64 );

		static constexpr uint	lanes	= 1;
		static constexpr uint	count	= sizeof(Native_t) / sizeof(IntType);
		using Array_t					= StaticArray< Scalar_t, count >;
		StaticAssert( sizeof(Array_t) == sizeof(Native_t) );

		using Signed_t		= SimdTInt64< ToSignedInteger< IntType >>;
		using Unsigned_t	= SimdTInt64< ToUnsignedInteger< IntType >>;
		using Shift64_t		= SimdTInt64< sint >;
		using Mask_t		= Base::_hidden_::MSBMask< count >;

	private:
		using _InvSign_t	= Conditional< IsSigned<IntType>, Unsigned_t, Signed_t >;


	// variables
	private:
		Native_t	_value;

		static constexpr bool	isU8	= IsSame< IntType, ubyte >;
		static constexpr bool	isU16	= IsSame< IntType, ushort >;
		static constexpr bool	isU32	= IsSame< IntType, uint >;

		static constexpr bool	isI8	= IsSame< IntType, sbyte >;
		static constexpr bool	isI16	= IsSame< IntType, sshort >;
		static constexpr bool	isI32	= IsSame< IntType, sint >;


	// methods
	public:
		SimdTInt64 ()										__NE___	: _value{} {}
		SimdTInt64 (Zero_t)									__NE___ : SimdTInt64{ IntType{0} } {}
		SimdTInt64 (UMax_t)									__NE___	: SimdTInt64{ ~IntType{0} } {}
		explicit SimdTInt64 (const Native_t &val)			__NE___	: _value{ val } {}
		explicit SimdTInt64 (const Scalar_t* ptr)			__NE___;
		explicit SimdTInt64 (Scalar_t val)					__NE___;

	  #ifdef AE_SIMD_SimdHalf4
		template <typename T = Scalar_t, ENABLEIF( sizeof(T)==2 )>
		explicit SimdTInt64 (const SimdHalf4::Bool4 &v)		__NE___ : _value{ v.Ref() } {}
	  #endif

		template <typename T = Scalar_t, ENABLEIF( sizeof(T)==1 )>
		SimdTInt64 (Scalar_t v0, Scalar_t v1, Scalar_t v2, Scalar_t v3,
					Scalar_t v4, Scalar_t v5, Scalar_t v6, Scalar_t v7) __NE___;

		template <typename T = Scalar_t, ENABLEIF( sizeof(T)==2 )>
		SimdTInt64 (Scalar_t v0, Scalar_t v1, Scalar_t v2, Scalar_t v3) __NE___;

		template <typename T = Scalar_t, ENABLEIF( sizeof(T)==4 )>
		SimdTInt64 (Scalar_t v0, Scalar_t v1)				__NE___;


		template <typename T = Scalar_t, ENABLEIF( IsUnsignedInteger<T> )>
		explicit SimdTInt64 (bool v)						__NE___ : SimdTInt64{ v ? Scalar_t{UMax} : Scalar_t{0} } {}

		template <typename B, typename T=Scalar_t, ENABLEIF( IsSame< T, ubyte > and IsSame< B, bool >)>
		SimdTInt64 (B v0, B v1, B v2, B v3, B v4, B v5, B v6, B v7)	__NE___;

		template <typename B, typename T=Scalar_t, ENABLEIF( IsSame< T, ushort > and IsSame< B, bool >)>
		SimdTInt64 (B v0, B v1, B v2, B v3)					__NE___;

		template <typename B, typename T=Scalar_t, ENABLEIF( IsSame< T, uint > and IsSame< B, bool >)>
		SimdTInt64 (B v0, B v1)								__NE___;


		template <typename T = Scalar_t, ENABLEIF( IsSignedInteger<T> )>
		ND_ Self	operator -  ()							C_NE___	{ return Negative(); }

		template <typename T = Scalar_t, ENABLEIF( IsUnsignedInteger<T> )>
		ND_ Bool_t	operator !  ()							C_NE___	{ return BitInverse(); }

		ND_ Self	operator ~  ()							C_NE___	{ return BitInverse(); }

		ND_ Self	operator +  (const Self &rhs)			C_NE___	{ return Add( rhs ); }
		ND_ Self	operator -  (const Self &rhs)			C_NE___	{ return Sub( rhs ); }
		ND_ Self	operator *  (const Self &rhs)			C_NE___	{ return Mul( rhs ); }

		ND_ Self	operator +  (Scalar_t rhs)				C_NE___	{ return Add( rhs ); }
		ND_ Self	operator -  (Scalar_t rhs)				C_NE___	{ return Sub( rhs ); }
		ND_ Self	operator *  (Scalar_t rhs)				C_NE___	{ return Mul( rhs ); }

		ND_ Self	operator &  (const Self &rhs)			C_NE___	{ return And( rhs ); }
		ND_ Self	operator |  (const Self &rhs)			C_NE___	{ return Or( rhs ); }
		ND_ Self	operator ^  (const Self &rhs)			C_NE___	{ return Xor( rhs ); }

		ND_ Self	operator &  (Scalar_t rhs)				C_NE___	{ return And( rhs ); }
		ND_ Self	operator |  (Scalar_t rhs)				C_NE___	{ return Or( rhs ); }
		ND_ Self	operator ^  (Scalar_t rhs)				C_NE___	{ return Xor( rhs ); }

		ND_ Self	operator << (uint shift)				C_NE___	{ return LShift( shift ); }
		ND_ Self	operator << (const Unsigned_t &shift)	C_NE___	{ return LShift( shift ); }

		ND_ Self	operator >> (uint shift)				C_NE___	{ return RShift( shift ); }
		ND_ Self	operator >> (const Unsigned_t &shift)	C_NE___	{ return RShift( shift ); }

		ND_ Bool_t	operator == (const Self &rhs)			C_NE___	{ return Equal( rhs ); }
		ND_ Bool_t	operator != (const Self &rhs)			C_NE___	{ return NotEqual( rhs ); }
		ND_ Bool_t	operator >  (const Self &rhs)			C_NE___	{ return Greater( rhs ); }
		ND_ Bool_t	operator <  (const Self &rhs)			C_NE___	{ return Less( rhs ); }
		ND_ Bool_t	operator >= (const Self &rhs)			C_NE___	{ return GEqual( rhs ); }
		ND_ Bool_t	operator <= (const Self &rhs)			C_NE___	{ return LEqual( rhs ); }

		template <uint I> ND_ Scalar_t	get ()				C_NE___;
		template <uint I> ND_ Self		set (Scalar_t val)	C_NE___;

		ND_ Native_t&		Ref ()							__NE___	{ return _value; }
		ND_ Native_t const&	Ref ()							C_NE___	{ return _value; }

	  #if AE_SIMD_NEON64
		ND_ Scalar_t	PrefixSum ()						C_NE___;
		ND_ auto		PrefixSumExt ()						C_NE___;
		ND_ Scalar_t	PrefixMax ()						C_NE___;
		ND_ Scalar_t	PrefixMin ()						C_NE___;
	  #endif

		template <typename T = Scalar_t, ENABLEIF( IsSignedInteger<T> )>
		ND_ Self	Negative ()								C_NE___;	// -x

		template <typename T = Scalar_t, ENABLEIF( IsSignedInteger<T> )>
		ND_ Self	NegativeSat ()							C_NE___;	// -x

		template <typename T = Scalar_t, ENABLEIF( IsSignedInteger<T> )>
		ND_ Self	Abs ()									C_NE___;	// abs(x)

		ND_ Self	Add (const Self &rhs)					C_NE___;
		ND_ Self	Sub (const Self &rhs)					C_NE___;
		ND_ Self	Mul (const Self &rhs)					C_NE___;
		ND_ Self	Min (const Self &rhs)					C_NE___;
		ND_ Self	Max (const Self &rhs)					C_NE___;

		ND_ Self	Add (Scalar_t rhs)						C_NE___	{ return Add( Self{rhs} );}
		ND_ Self	Sub (Scalar_t rhs)						C_NE___	{ return Sub( Self{rhs} );}
		ND_ Self	Mul (Scalar_t rhs)						C_NE___;
		ND_ Self	Min (Scalar_t rhs)						C_NE___	{ return Min( Self{rhs} ); }
		ND_ Self	Max (Scalar_t rhs)						C_NE___	{ return Max( Self{rhs} ); }

		ND_ Self	AddSat (const Self &rhs)				C_NE___;
		ND_ Self	SubSat (const Self &rhs)				C_NE___;

	  #if AE_SIMD_NEON64
		ND_ Self	AddSat (const _InvSign_t &rhs)			C_NE___;
	  #endif

		ND_ Bool_t	Equal    (const Self &rhs)				C_NE___;
		ND_ Bool_t	NotEqual (const Self &rhs)				C_NE___	{ return not Equal( rhs ); }
		ND_ Bool_t  Greater  (const Self &rhs)				C_NE___;
		ND_ Bool_t  GEqual   (const Self &rhs)				C_NE___;
		ND_ Bool_t  Less     (const Self &rhs)				C_NE___;
		ND_ Bool_t  LEqual   (const Self &rhs)				C_NE___;

		ND_ Bool_t	IsZero ()								C_NE___;
		ND_ Bool_t	IsNotZero ()							C_NE___	{ return ~IsZero(); }

		friend Self  MulAdd    (const Self &a, const Self &b, const Self &c) __NE___;	// (a * b) + c
		friend Self  MulAdd    (const Self &a, Scalar_t    b, const Self &c) __NE___;	// (a * b) + c
		friend Self  NegMulAdd (const Self &a, const Self &b, const Self &c) __NE___;	// c - (a * b)
		friend Self  NegMulAdd (const Self &a, Scalar_t    b, const Self &c) __NE___;	// c - (a * b)

		ND_ friend Self  Select (const Bool_t &condition, const Self &ifTrue, const Self &ifFalse) __NE___ { return _Select( condition, ifTrue, ifFalse ); }

		template <typename T = Scalar_t, ENABLEIF( IsUnsignedInteger<T> )>
		ND_ Mask_t	ToBitfield ()							C_NE___;

		template <typename T = Scalar_t, ENABLEIF( IsUnsignedInteger<T> )>
		ND_ bool	All ()									C_NE___;

		template <typename T = Scalar_t, ENABLEIF( IsUnsignedInteger<T> )>
		ND_ bool	Any ()									C_NE___;

		template <typename T = Scalar_t, ENABLEIF( IsUnsignedInteger<T> )>
		ND_ bool	None ()									C_NE___;


	// bit shift //
		template <uint Shift>
		ND_ Self	LShift_Logic ()							C_NE___;
		ND_ Self	LShift_Logic (uint shift)				C_NE___	{ return _LShift_Logic( shift ); }
		ND_ Self	LShift_Logic (const Shift64_t &shift)	C_NE___	{ return _LShift_Logic( shift.get<0>() ); }

		template <uint Shift>
		ND_ Self	LShift_Arith ()							C_NE___;
		ND_ Self	LShift_Arith (uint shift)				C_NE___	{ return _LShift_Arith( shift ); }
		ND_ Self	LShift_Arith (const Shift64_t &shift)	C_NE___	{ return _LShift_Arith( shift.get<0>() ); }

		template <uint Shift>
		ND_ Self	RShift_Logic ()							C_NE___;
		ND_ Self	RShift_Logic (uint shift)				C_NE___	{ return _LShift_Logic( -int(shift) ); }
		ND_ Self	RShift_Logic (const Shift64_t &shift)	C_NE___	{ return _LShift_Logic( -shift.get<0>() ); }

		template <uint Shift>
		ND_ Self	RShift_Arith ()							C_NE___;
		ND_ Self	RShift_Arith (uint shift)				C_NE___	{ return _LShift_Arith( -int(shift) ); }
		ND_ Self	RShift_Arith (const Shift64_t &shift)	C_NE___	{ return _LShift_Arith( -shift.get<0>() ); }

		ND_ Self	LShift_LogicV (const Unsigned_t &shift)	C_NE___	{ return _LShift_LogicV( shift.template BitCast<Signed_t>() ); }
		ND_ Self	RShift_LogicV (const Unsigned_t &shift)	C_NE___	{ return _LShift_LogicV( shift.template BitCast<Signed_t>().Negative() ); }
		ND_ Self	LShift_ArithV (const Unsigned_t &shift)	C_NE___	{ return _LShift_ArithV( shift.template BitCast<Signed_t>() ); }
		ND_ Self	RShift_ArithV (const Unsigned_t &shift)	C_NE___	{ return _LShift_ArithV( shift.template BitCast<Signed_t>().Negative() ); }

		template <uint Shift>	ND_ Self  LShift ()					C_NE___	{ return RShift_Arith< Shift >(); }
		template <uint Shift>	ND_ Self  RShift ()					C_NE___	{ return RShift_Arith< Shift >(); }
		template <typename T>	ND_ Self  LShift (const T &shift)	C_NE___;
		template <typename T>	ND_ Self  RShift (const T &shift)	C_NE___;


	// per bit operations //
		ND_ Self	BitInverse ()							C_NE___;	// ~a

		ND_ Self	And (const Self &rhs)					C_NE___;	// a & b
		ND_ Self	Or  (const Self &rhs)					C_NE___;	// a | b
		ND_ Self	Xor (const Self &rhs)					C_NE___;	// a ^ b
		ND_ Self	OrNot (const Self &rhs)					C_NE___;	// a | ~b
		ND_ Self	AndNot (const Self &rhs)				C_NE___;	// ~a & b
		ND_ Self	AllBits (const Self &mask)				C_NE___;	// (a & mask) != 0

		ND_ Self	And (Scalar_t rhs)						C_NE___	{ return And( Self{rhs} ); }
		ND_ Self	Or  (Scalar_t rhs)						C_NE___	{ return Or(  Self{rhs} ); }
		ND_ Self	Xor (Scalar_t rhs)						C_NE___	{ return Xor( Self{rhs} ); }
		ND_ Self	OrNot (Scalar_t rhs)					C_NE___	{ return OrNot( Self{rhs} ); }
		ND_ Self	AndNot (Scalar_t rhs)					C_NE___	{ return AndNot( Self{rhs} ); }
		ND_ Self	AllBits (Scalar_t mask)					C_NE___	{ return AllBits( Self{mask} ); }


	// conversion //
		template <uint Idx>
		ND_ Self	Lane ()									C_NE___ { StaticAssert( Idx < lanes );  return *this; }

		template <uint X, uint Y, uint Z, uint W,  typename T = Scalar_t, ENABLEIF( sizeof(T)==2 )>
		ND_ Self	Swizzle ()								C_NE___;

		template <uint X, uint Y,  typename T = Scalar_t, ENABLEIF( sizeof(T)==4 )>
		ND_ Self	Swizzle ()								C_NE___;

		template <typename DstScalar>
		ND_ auto	Convert ()								C_NE___;

		template <typename DstType>
		ND_ DstType	BitCast ()								C_NE___;

		void		ToArray (OUT Scalar_t* dst)				C_NE___;
		ND_ Array_t	ToArray ()								C_NE___	{ Array_t arr;  ToArray( OUT arr.data() );  return arr; }

		template <typename T = Scalar_t, ENABLEIF( IsUnsignedInteger<T> )>
		ND_ auto	ToSigned ()								C_NE___;

		template <typename T = Scalar_t, ENABLEIF( IsSignedInteger<T> )>
		ND_ auto	ToUnsigned ()							C_NE___;

		template <uint I=0>	ND_ auto	ToFloat ()			C_NE___;
							ND_ auto	ToHalf ()			C_NE___;
							ND_ auto	ToByte ()			C_NE___;
							ND_ auto	ToShort ()			C_NE___;
		template <uint I=0>	ND_ auto	ToInt ()			C_NE___;
		template <uint I=0>	ND_ auto	ToLong ()			C_NE___;

		template <typename T = Scalar_t, ENABLEIF( sizeof(T)>=2 )>
		ND_ explicit operator PackedVec<T,count> ()			C_NE___
		{
			StaticAssert( count <= 4 );
			PackedVec<T,count> tmp;
			ToArray( OUT &tmp.x );
			return tmp;
		}


	// Features //
		NdCe__ static bool  Has_Arithmetic ()				{ return true; }
		NdCe__ static bool  Has_VecLShift_Logic ()			{ return true; }
		NdCe__ static bool  Has_VecRShift_Logic ()			{ return true; }
		NdCe__ static bool  Has_VecLShift_Arithmetic ()		{ return true; }
		NdCe__ static bool  Has_VecRShift_Arithmetic ()		{ return true; }
		NdCe__ static bool  Has_ScalarShift_Logic ()		{ return true; }
		NdCe__ static bool  Has_ScalarShift_Arithmetic ()	{ return true; }
	  #if AE_SIMD_NEON64
		NdCe__ static bool  Has_PrefixSum ()				{ return true; }
		NdCe__ static bool  Has_PrefixSumExt ();
		NdCe__ static bool  Has_PrefixMinMax ();
	  #else
		NdCe__ static bool  Has_PrefixSum ()				{ return false; }
		NdCe__ static bool  Has_PrefixSumExt ()				{ return false; }
		NdCe__ static bool  Has_PrefixMinMax ()				{ return false; }
	  #endif
		NdCe__ static bool  Has_Mul ();
		NdCe__ static bool  Has_MulExt ()					{ return false; }
		NdCe__ static bool  Has_MulAdd ()					{ return true; }
		NdCe__ static bool  Has_Div ()						{ return false; }
		NdCe__ static bool  Has_Equal ()					{ return true; }
		NdCe__ static bool  Has_Greater ()					{ return true; }
		NdCe__ static bool  Has_MinMax ();
		NdCe__ static bool  Has_Swizzle ()					{ return sizeof(Scalar_t) >= sizeof(ushort); }
		NdCe__ static bool  Has_Shuffle ()					{ return true; }

		template <typename DstType>	NdCe__ static bool  Has_BitCast ();
		template <typename DstType>	NdCe__ static bool  Has_Convert ();

	private:
		ND_ static Self  _Select (const Bool_t &condition, const Self &ifTrue, const Self &ifFalse) __NE___;

		template <typename T>	ND_ Self  _LShift_Logic (T shift)	C_NE___	{ return _LShift_LogicV( Signed_t{ typename Signed_t::Scalar_t( shift )}); }
		template <typename T>	ND_ Self  _LShift_Arith (T shift)	C_NE___	{ return _LShift_ArithV( Signed_t{ typename Signed_t::Scalar_t( shift )}); }

		ND_ Self	_LShift_LogicV (const Signed_t &shift)			C_NE___;
		ND_ Self	_LShift_ArithV (const Signed_t &shift)			C_NE___;
	};
//-----------------------------------------------------------------------------



	//
	// 128 bit integer (Neon)
	//
	#define AE_SIMD_SimdTInt128
	template <typename IntType>
	struct SimdTInt128
	{
		StaticAssert( IsInteger<IntType> );

	// types
	public:
		using Scalar_t	= IntType;
		using Self		= SimdTInt128< IntType >;
		using Bool_t	= SimdTInt128< ToUnsignedInteger< IntType >>;
		using Native_t	= Conditional< IsSame< IntType, sbyte >, int8x16_t,
							Conditional< IsSame< IntType, ubyte >, uint8x16_t,
								Conditional< IsSame< IntType, sshort >, int16x8_t,
									Conditional< IsSame< IntType, ushort >, uint16x8_t,
										Conditional< IsSame< IntType, sint >, int32x4_t,
											Conditional< IsSame< IntType, uint >, uint32x4_t,
												Conditional< IsSame< IntType, slong >, int64x2_t,
													Conditional< IsSame< IntType, ulong >, uint64x2_t, void >>>>>>>>;
		StaticAssert( CT_SizeOfInBits<Native_t> == 128 );

		static constexpr uint	lanes	= sizeof(IntType) >= sizeof(ulong) ? 1 : 2;
		static constexpr uint	count	= sizeof(Native_t) / sizeof(IntType);
		using Array_t					= StaticArray< Scalar_t, count >;
		StaticAssert( sizeof(Array_t) == sizeof(Native_t) );

		using Signed_t		= SimdTInt128< ToSignedInteger< IntType >>;
		using Unsigned_t	= SimdTInt128< ToUnsignedInteger< IntType >>;
		using Shift64_t		= SimdTInt128< slong >;
		using Mask_t		= Base::_hidden_::MSBMask< count >;

	private:
		using _InvSign_t	= Conditional< IsSigned<IntType>, Unsigned_t, Signed_t >;


	// variables
	private:
		Native_t	_value;

		static constexpr bool	isU8	= IsSame< IntType, ubyte >;
		static constexpr bool	isU16	= IsSame< IntType, ushort >;
		static constexpr bool	isU32	= IsSame< IntType, uint >;
		static constexpr bool	isU64	= IsSame< IntType, ulong >;

		static constexpr bool	isI8	= IsSame< IntType, sbyte >;
		static constexpr bool	isI16	= IsSame< IntType, sshort >;
		static constexpr bool	isI32	= IsSame< IntType, sint >;
		static constexpr bool	isI64	= IsSame< IntType, slong >;


	// methods
	public:
		SimdTInt128 ()										__NE___	: _value{} {}
		SimdTInt128 (Zero_t)								__NE___ : SimdTInt128{ IntType{0} } {}
		SimdTInt128 (UMax_t)								__NE___	: SimdTInt128{ ~IntType{0} } {}
		explicit SimdTInt128 (const Native_t &val)			__NE___	: _value{ val } {}
		explicit SimdTInt128 (const Scalar_t* ptr)			__NE___;
		explicit SimdTInt128 (Scalar_t val)					__NE___;

		template <typename T = Scalar_t, ENABLEIF( sizeof(T)<=4 )>
		explicit SimdTInt128 (const SimdTInt64<IntType> &low)	__NE___ : SimdTInt128{ low, SimdTInt64<IntType>{} } {}

		template <typename T = Scalar_t, ENABLEIF( sizeof(T)<=4 )>
		explicit SimdTInt128 (const SimdTInt64<IntType> &low,
							  const SimdTInt64<IntType> &high)	__NE___;


		template <typename T = Scalar_t, ENABLEIF( sizeof(T)==1 )>
		SimdTInt128 (Scalar_t v00, Scalar_t v01, Scalar_t v02, Scalar_t v03,
					 Scalar_t v04, Scalar_t v05, Scalar_t v06, Scalar_t v07,
					 Scalar_t v08, Scalar_t v09, Scalar_t v10, Scalar_t v11,
					 Scalar_t v12, Scalar_t v13, Scalar_t v14, Scalar_t v15) __NE___;

		template <typename T = Scalar_t, ENABLEIF( sizeof(T)==2 )>
		SimdTInt128 (Scalar_t v0, Scalar_t v1, Scalar_t v2, Scalar_t v3,
					 Scalar_t v4, Scalar_t v5, Scalar_t v6, Scalar_t v7) __NE___;

		template <typename T = Scalar_t, ENABLEIF( sizeof(T)==4 )>
		SimdTInt128 (Scalar_t v0, Scalar_t v1, Scalar_t v2, Scalar_t v3) __NE___;

		template <typename T = Scalar_t, ENABLEIF( sizeof(T)==8 )>
		SimdTInt128 (Scalar_t v0, Scalar_t v1)				__NE___;


		template <typename T = Scalar_t, ENABLEIF( IsUnsignedInteger<T> )>
		explicit SimdTInt128 (bool v)						__NE___ : SimdTInt128{ v ? Scalar_t{UMax} : Scalar_t{0} } {}

		template <typename B, typename T=Scalar_t, ENABLEIF( IsSame< T, ubyte > and IsSame< B, bool >)>
		SimdTInt128 (B v00, B v01, B v02, B v03, B v04, B v05, B v06, B v07,
					 B v08, B v09, B v10, B v11, B v12, B v13, B v14, B v15) __NE___;

		template <typename B, typename T=Scalar_t, ENABLEIF( IsSame< T, ushort > and IsSame< B, bool >)>
		SimdTInt128 (B v0, B v1, B v2, B v3, B v4, B v5, B v6, B v7) __NE___;

		template <typename B, typename T=Scalar_t, ENABLEIF( IsSame< T, uint > and IsSame< B, bool >)>
		SimdTInt128 (B v0, B v1, B v2, B v3)				__NE___;

		template <typename B, typename T=Scalar_t, ENABLEIF( IsSame< T, ulong > and IsSame< B, bool >)>
		SimdTInt128 (B v0, B v1)							__NE___;


		template <typename T = Scalar_t, ENABLEIF( IsSignedInteger<T> )>
		ND_ Self	operator - ()							C_NE___	{ return Negative(); }

		template <typename T = Scalar_t, ENABLEIF( IsUnsignedInteger<T> )>
		ND_ Bool_t	operator ! ()							C_NE___	{ return BitInverse(); }

		ND_ Self	operator ~ ()							C_NE___	{ return BitInverse(); }

		ND_ Self	operator +  (const Self &rhs)			C_NE___	{ return Add( rhs ); }
		ND_ Self	operator -  (const Self &rhs)			C_NE___	{ return Sub( rhs ); }
		ND_ Self	operator *  (const Self &rhs)			C_NE___	{ return Mul( rhs ); }

		ND_ Self	operator +  (Scalar_t rhs)				C_NE___	{ return Add( rhs ); }
		ND_ Self	operator -  (Scalar_t rhs)				C_NE___	{ return Sub( rhs ); }
		ND_ Self	operator *  (Scalar_t rhs)				C_NE___	{ return Mul( rhs ); }

		ND_ Self	operator &  (const Self &rhs)			C_NE___	{ return And( rhs ); }
		ND_ Self	operator |  (const Self &rhs)			C_NE___	{ return Or( rhs ); }
		ND_ Self	operator ^  (const Self &rhs)			C_NE___	{ return Xor( rhs ); }

		ND_ Self	operator &  (Scalar_t rhs)				C_NE___	{ return And( rhs ); }
		ND_ Self	operator |  (Scalar_t rhs)				C_NE___	{ return Or( rhs ); }
		ND_ Self	operator ^  (Scalar_t rhs)				C_NE___	{ return Xor( rhs ); }

		ND_ Bool_t	operator == (const Self &rhs)			C_NE___	{ return Equal( rhs ); }
		ND_ Bool_t	operator != (const Self &rhs)			C_NE___	{ return NotEqual( rhs ); }
		ND_ Bool_t	operator >  (const Self &rhs)			C_NE___	{ return Greater( rhs ); }
		ND_ Bool_t	operator <  (const Self &rhs)			C_NE___	{ return Less( rhs ); }
		ND_ Bool_t	operator >= (const Self &rhs)			C_NE___	{ return GEqual( rhs ); }
		ND_ Bool_t	operator <= (const Self &rhs)			C_NE___	{ return LEqual( rhs ); }

		ND_ Self	operator << (uint shift)				C_NE___	{ return LShift( shift ); }
		ND_ Self	operator << (const Shift64_t &shift)	C_NE___	{ return LShift( shift ); }
		ND_ Self	operator << (const Unsigned_t &shift)	C_NE___	{ return LShift( shift ); }

		ND_ Self	operator >> (uint shift)				C_NE___	{ return RShift( shift ); }
		ND_ Self	operator >> (const Shift64_t &shift)	C_NE___	{ return RShift( shift ); }
		ND_ Self	operator >> (const Unsigned_t &shift)	C_NE___	{ return RShift( shift ); }

		template <uint I> ND_ Scalar_t	get ()				C_NE___;
		template <uint I> ND_ Self		set (Scalar_t)		C_NE___;

		ND_ Native_t&		Ref ()							__NE___	{ return _value; }
		ND_ Native_t const&	Ref ()							C_NE___	{ return _value; }

	  #if AE_SIMD_NEON64
		ND_ Scalar_t	PrefixSum ()						C_NE___;
		ND_ auto		PrefixSumExt ()						C_NE___;
		ND_ Scalar_t	PrefixMax ()						C_NE___;
		ND_ Scalar_t	PrefixMin ()						C_NE___;
	  #endif

		ND_ Self	Add (const Self &rhs)					C_NE___;
		ND_ Self	Sub (const Self &rhs)					C_NE___;
		ND_ Self	Mul (const Self &rhs)					C_NE___;
		ND_ Self	Min (const Self &rhs)					C_NE___;
		ND_ Self	Max (const Self &rhs)					C_NE___;

		ND_ Self	Add (Scalar_t rhs)						C_NE___	{ return Add( Self{rhs} ); }
		ND_ Self	Sub (Scalar_t rhs)						C_NE___	{ return Sub( Self{rhs} ); }
		ND_ Self	Mul (Scalar_t rhs)						C_NE___;
		ND_ Self	Min (Scalar_t rhs)						C_NE___	{ return Min( Self{rhs} ); }
		ND_ Self	Max (Scalar_t rhs)						C_NE___	{ return Max( Self{rhs} ); }

		ND_ Self	AddSat (const Self &rhs)				C_NE___;
		ND_ Self	SubSat (const Self &rhs)				C_NE___;

	  #if AE_SIMD_NEON64
		ND_ Self	AddSat (const _InvSign_t &rhs)			C_NE___;
	  #endif

		ND_ Bool_t	Equal (const Self &rhs)					C_NE___;
		ND_ Bool_t	NotEqual (const Self &rhs)				C_NE___	{ return not Equal( rhs ); }
		ND_ Bool_t	Greater (const Self &rhs)				C_NE___;
		ND_ Bool_t	Less (const Self &rhs)					C_NE___;
		ND_ Bool_t	LEqual (const Self &rhs)				C_NE___;
		ND_ Bool_t	GEqual (const Self &rhs)				C_NE___;

		ND_ Bool_t	IsZero ()								C_NE___;
		ND_ Bool_t	IsNotZero ()							C_NE___	{ return ~IsZero(); }

		template <typename T = Scalar_t, ENABLEIF( IsSignedInteger<T> )>
		ND_ Self	Abs ()									C_NE___;	// abs(x)

		template <typename T = Scalar_t, ENABLEIF( IsSignedInteger<T> )>
		ND_ Self	Negative ()								C_NE___;	// -x

		template <typename T = Scalar_t, ENABLEIF( IsSignedInteger<T> )>
		ND_ Self	NegativeSat ()							C_NE___;	// -x

		friend Self  MulAdd    (const Self &a, const Self &b, const Self &c) __NE___;	// (a * b) + c
		friend Self  MulAdd    (const Self &a, Scalar_t    b, const Self &c) __NE___;	// (a * b) + c
		friend Self  NegMulAdd (const Self &a, const Self &b, const Self &c) __NE___;	// c - (a * b)
		friend Self  NegMulAdd (const Self &a, Scalar_t    b, const Self &c) __NE___;	// c - (a * b)

		ND_ friend Self  Select (const Bool_t &condition, const Self &ifTrue, const Self &ifFalse) __NE___ { return _Select( condition, ifTrue, ifFalse ); }

		template <typename T = Scalar_t, ENABLEIF( IsUnsignedInteger<T> )>
		ND_ Mask_t	ToBitfield ()							C_NE___;

		template <typename T = Scalar_t, ENABLEIF( IsUnsignedInteger<T> )>
		ND_ bool	All ()									C_NE___;

		template <typename T = Scalar_t, ENABLEIF( IsUnsignedInteger<T> )>
		ND_ bool	Any ()									C_NE___;

		template <typename T = Scalar_t, ENABLEIF( IsUnsignedInteger<T> )>
		ND_ bool	None ()									C_NE___;


	// bit shift //
		template <uint Shift>
		ND_ Self	LShift_Logic ()							C_NE___;
		ND_ Self	LShift_Logic (uint shift)				C_NE___	{ return _LShift_Logic( shift ); }
		ND_ Self	LShift_Logic (const Shift64_t &shift)	C_NE___	{ return _LShift_Logic( shift.get<0>() ); }

		template <uint Shift>
		ND_ Self	LShift_Arith ()							C_NE___;
		ND_ Self	LShift_Arith (uint shift)				C_NE___	{ return _LShift_Arith( shift ); }
		ND_ Self	LShift_Arith (const Shift64_t &shift)	C_NE___	{ return _LShift_Arith( shift.get<0>() ); }

		template <uint Shift>
		ND_ Self	RShift_Logic ()							C_NE___;
		ND_ Self	RShift_Logic (uint shift)				C_NE___	{ return _LShift_Logic( -int(shift) ); }
		ND_ Self	RShift_Logic (const Shift64_t &shift)	C_NE___	{ return _LShift_Logic( -shift.get<0>() ); }

		template <uint Shift>
		ND_ Self	RShift_Arith ()							C_NE___;
		ND_ Self	RShift_Arith (uint shift)				C_NE___	{ return _LShift_Arith( -int(shift) ); }
		ND_ Self	RShift_Arith (const Shift64_t &shift)	C_NE___	{ return _LShift_Arith( -shift.get<0>() ); }

		ND_ Self	LShift_LogicV (const Unsigned_t &shift)	C_NE___	{ return _LShift_LogicV( shift.template BitCast<Signed_t>() ); }
		ND_ Self	RShift_LogicV (const Unsigned_t &shift)	C_NE___	{ return _LShift_LogicV( shift.template BitCast<Signed_t>().Negative() ); }
		ND_ Self	LShift_ArithV (const Unsigned_t &shift)	C_NE___	{ return _LShift_ArithV( shift.template BitCast<Signed_t>() ); }
		ND_ Self	RShift_ArithV (const Unsigned_t &shift)	C_NE___	{ return _LShift_ArithV( shift.template BitCast<Signed_t>().Negative() ); }

		template <uint Shift>	ND_ Self  LShift ()					C_NE___	{ return RShift_Arith< Shift >(); }
		template <uint Shift>	ND_ Self  RShift ()					C_NE___	{ return RShift_Arith< Shift >(); }
		template <typename T>	ND_ Self  LShift (const T &shift)	C_NE___;
		template <typename T>	ND_ Self  RShift (const T &shift)	C_NE___;


	// per bit operations //
		ND_ Self	BitInverse ()							C_NE___;	// ~a

		ND_ Self	And (const Self &rhs)					C_NE___;	// a & b
		ND_ Self	Or (const Self &rhs)					C_NE___;	// a | b
		ND_ Self	Xor (const Self &rhs)					C_NE___;	// a ^ b
		ND_ Self	OrNot (const Self &rhs)					C_NE___;	// a | ~b
		ND_ Self	AndNot (const Self &rhs)				C_NE___;	// ~a & b
		ND_ Self	AllBits (const Self &mask)				C_NE___;	// (a & mask) != 0

		ND_ Self	And (Scalar_t rhs)						C_NE___	{ return And( Self{rhs} ); }
		ND_ Self	Or  (Scalar_t rhs)						C_NE___	{ return Or(  Self{rhs} ); }
		ND_ Self	Xor (Scalar_t rhs)						C_NE___	{ return Xor( Self{rhs} ); }
		ND_ Self	OrNot (Scalar_t rhs)					C_NE___	{ return OrNot( Self{rhs} ); }
		ND_ Self	AndNot (Scalar_t rhs)					C_NE___	{ return AndNot( Self{rhs} ); }
		ND_ Self	AllBits (Scalar_t mask)					C_NE___	{ return AllBits( Self{mask} ); }


	// conversion //
		template <uint Idx>
		ND_ auto	Lane ()									C_NE___;

		template <uint X, uint Y, uint Z, uint W,  typename T = Scalar_t, ENABLEIF( sizeof(T)==4 )>
		ND_ Self	Swizzle ()								C_NE___;

		template <uint X, uint Y,  typename T = Scalar_t, ENABLEIF( sizeof(T)==8 )>
		ND_ Self	Swizzle ()								C_NE___;

		void		ToArray (OUT Scalar_t* dst)				C_NE___;
		ND_ Array_t	ToArray ()								C_NE___	{ Array_t arr;  ToArray( OUT arr.data() );  return arr; }

		template <typename DstScalar>
		ND_ auto	Convert ()								C_NE___;

		template <typename DstType>
		ND_ DstType	BitCast ()								C_NE___;

		template <typename T = Scalar_t, ENABLEIF( IsUnsignedInteger<T> )>
		ND_ auto	ToSigned ()								C_NE___;

		template <typename T = Scalar_t, ENABLEIF( IsSignedInteger<T> )>
		ND_ auto	ToUnsigned ()							C_NE___;

		template <uint I=0>	ND_ auto	ToHalf ()			C_NE___;
		template <uint I=0>	ND_ auto	ToFloat ()			C_NE___;
		template <uint I=0>	ND_ auto	ToDouble ()			C_NE___;
							ND_ auto	ToByte ()			C_NE___;
		template <uint I=0>	ND_ auto	ToShort ()			C_NE___;
		template <uint I=0>	ND_ auto	ToInt ()			C_NE___;
		template <uint I=0>	ND_ auto	ToLong ()			C_NE___;

		template <typename T = Scalar_t, ENABLEIF( sizeof(T)>=4 )>
		ND_ explicit operator PackedVec<T,count> ()			C_NE___
		{
			StaticAssert( count <= 4 );
			PackedVec<T,count> tmp;
			ToArray( OUT &tmp.x );
			return tmp;
		}


	// Features //
		NdCe__ static bool  Has_Arithmetic ()				{ return true; }
		NdCe__ static bool  Has_VecLShift_Logic ()			{ return true; }
		NdCe__ static bool  Has_VecRShift_Logic ()			{ return true; }
		NdCe__ static bool  Has_VecLShift_Arithmetic ()		{ return true; }
		NdCe__ static bool  Has_VecRShift_Arithmetic ()		{ return true; }
		NdCe__ static bool  Has_ScalarShift_Logic ()		{ return true; }
		NdCe__ static bool  Has_ScalarShift_Arithmetic ()	{ return true; }
	  #if AE_SIMD_NEON64
		NdCe__ static bool  Has_PrefixSum ()				{ return true; }
		NdCe__ static bool  Has_PrefixSumExt ();
		NdCe__ static bool  Has_PrefixMinMax ();
	  #else
		NdCe__ static bool  Has_PrefixSum ()				{ return false; }
		NdCe__ static bool  Has_PrefixSumExt ()				{ return false; }
		NdCe__ static bool  Has_PrefixMinMax ()				{ return false; }
	  #endif
		NdCe__ static bool  Has_Mul ();
		NdCe__ static bool  Has_MulExt ()					{ return false; }
		NdCe__ static bool  Has_MulAdd ()					{ return true; }
		NdCe__ static bool  Has_Div ()						{ return false; }
		NdCe__ static bool  Has_Equal ()					{ return AE_SIMD_NEON64 or (sizeof(Scalar_t) <= sizeof(uint)); }
		NdCe__ static bool  Has_Greater ()					{ return AE_SIMD_NEON64 or (sizeof(Scalar_t) <= sizeof(uint)); }
		NdCe__ static bool  Has_MinMax ();
		NdCe__ static bool  Has_Swizzle ()					{ return sizeof(Scalar_t) >= sizeof(uint); }
		NdCe__ static bool  Has_Shuffle ()					{ return true; }

		template <typename DstType>	NdCe__ static bool  Has_BitCast ();
		template <typename DstType>	NdCe__ static bool  Has_Convert ();

	private:
		ND_ static Self  _Select (const Bool_t &condition, const Self &ifTrue, const Self &ifFalse) __NE___;

		template <typename T>	ND_ Self  _LShift_Logic (T shift)	C_NE___	{ return _LShift_LogicV( Signed_t{ typename Signed_t::Scalar_t( shift )}); }
		template <typename T>	ND_ Self  _LShift_Arith (T shift)	C_NE___	{ return _LShift_ArithV( Signed_t{ typename Signed_t::Scalar_t( shift )}); }

		ND_ Self	_LShift_LogicV (const Signed_t &shift)			C_NE___;
		ND_ Self	_LShift_ArithV (const Signed_t &shift)			C_NE___;
	};
//-----------------------------------------------------------------------------



# if AE_SIMD_NEON_HALF

	//
	// 128 bit half float (Neon)
	//
	#define AE_SIMD_SimdHalf8
	struct SimdHalf8
	{
	// types
	public:
		static constexpr uint	lanes	= 2;
		static constexpr uint	count	= 8;

		using Scalar_t		= half;
		using NativeScalar_t= float16_t;
		using Self			= SimdHalf8;
		using Native_t		= float16x8_t;
		using Array_t		= StaticArray< Scalar_t, count >;
		using SimdInt_t		= SimdShort8;
		using SimdUInt_t	= SimdUShort8;
		using Mask_t		= Base::_hidden_::MSBMask< count >;

		StaticAssert( sizeof(Array_t) == sizeof(Native_t) );


		struct Bool8
		{
		private:
			uint16x8_t	_value;

		public:
			explicit Bool8 (bool val)							__NE___ : _value{ vdupq_n_u16( val ? -1 : 0 )} {}
			explicit Bool8 (const uint16x8_t &v)				__NE___	: _value{ v } {}
			explicit Bool8 (const SimdInt_t &v)					__NE___	: _value{ v.BitCast<SimdUInt_t>().Ref() } {}
			explicit Bool8 (const SimdUInt_t &v)				__NE___	: _value{ v.Ref() } {}

			Bool8 (bool v0, bool v1, bool v2, bool v3,
				   bool v4, bool v5, bool v6, bool v7)			__NE___ : Bool8{ SimdUInt_t{ v0, v1, v2, v3, v4, v5, v6, v7 }} {}

			ND_ Bool8	operator | (const Bool8 &rhs)			C_NE___	{ return Bool8{ vorrq_u16( _value, rhs._value )}; }
			ND_ Bool8	operator & (const Bool8 &rhs)			C_NE___	{ return Bool8{ vandq_u16( _value, rhs._value )}; }
			ND_ Bool8	operator ^ (const Bool8 &rhs)			C_NE___	{ return Bool8{ veorq_u16( _value, rhs._value )}; }
			ND_ Bool8	operator ! ()							C_NE___	{ return Bool8{ vmvnq_u16( _value )}; }

			ND_ Bool8	operator == (const Bool8 &rhs)			C_NE___	{ return Bool8{ vandq_u16( _value, rhs._value )}; }
			ND_ Bool8	operator != (const Bool8 &rhs)			C_NE___	{ return Bool8{ vbicq_u16( rhs._value, _value )}; }

			template <uint I> ND_ bool	get ()					C_NE___	{ StaticAssert( I < count );  return vgetq_lane_u16( _value, I ) == UMax; }

			ND_ bool	All ()									C_NE___	{ return SimdUInt_t{ _value }.All(); }
			ND_ bool	Any ()									C_NE___	{ return SimdUInt_t{ _value }.Any(); }
			ND_ bool	None ()									C_NE___	{ return SimdUInt_t{ _value }.None(); }

			ND_ auto&	Ref ()									C_NE___	{ return _value; }

			ND_ Mask_t	ToBitfield ()							C_NE___	{ return SimdUInt_t{ _value }.ToBitfield(); }

			template <typename DstType>
			ND_ DstType	BitCast ()								C_NE___;

			ND_ explicit operator SimdInt_t ()					C_NE___	{ return SimdUInt_t{ _value }.BitCast<SimdInt_t>(); }
			ND_ explicit operator SimdUInt_t ()					C_NE___	{ return SimdUInt_t{ _value }; }

			ND_ static Bool8	True ()							__NE___	{ return Bool8{ vdupq_n_u16( UMax )}; }
			ND_ static Bool8	False ()						__NE___	{ return Bool8{ vdupq_n_u16( 0 )}; }
		};
		using Bool_t = Bool8;


	// variables
	private:
		Native_t	_value;		// half[8]

		static constexpr ushort		c_One_BitMask = 0x3c00;	// bitcast<ushort>( 1.hf )


	// methods
	public:
		SimdHalf8 ()											__NE___	: SimdHalf8{ float16_t{0} } {}
		SimdHalf8 (Zero_t)										__NE___	: SimdHalf8{ float16_t{0} } {}
		SimdHalf8 (float16x4_t low, float16x4_t high)			__NE___	: _value{ vcombine_f16( low, high )} {}
		explicit SimdHalf8 (Scalar_t val)						__NE___	: SimdHalf8{ Base::BitCast<float16_t>( val )} {}
		explicit SimdHalf8 (float16_t val)						__NE___	: _value{ vdupq_n_f16( val )} {}
		explicit SimdHalf8 (const float16_t* ptr)				__NE___	: _value{ vld1q_f16( GetNonNull( ptr ))} {}
		explicit SimdHalf8 (const Scalar_t* ptr)				__NE___	: SimdHalf8{ Base::Cast<float16_t>( ptr )} {}
		explicit SimdHalf8 (const Native_t &val)				__NE___	: _value{ val } {}
		explicit SimdHalf8 (const Bool8 &v)						__NE___ : _value{ vreinterpretq_f16_u16( vandq_u16( v.Ref(), vdupq_n_u16(c_One_BitMask) ))} {}

		explicit SimdHalf8 (const SimdHalf4 &low)							__NE___ : SimdHalf8{ low, SimdHalf4{} } {}
		explicit SimdHalf8 (const SimdFloat4 &low)							__NE___;

		explicit SimdHalf8 (const SimdHalf4 &low, const SimdHalf4 &high)	__NE___ : SimdHalf8{ low._value, high._value } {}
		explicit SimdHalf8 (const SimdFloat4 &low, const SimdFloat4 &high)	__NE___;

		SimdHalf8 (Scalar_t v0, Scalar_t v1, Scalar_t v2, Scalar_t v3,
				   Scalar_t v4, Scalar_t v5, Scalar_t v6, Scalar_t v7)		__NE___;
		SimdHalf8 (float16_t v0, float16_t v1, float16_t v2, float16_t v3,
				   float16_t v4, float16_t v5, float16_t v6, float16_t v7)	__NE___;
		SimdHalf8 (float v0, float v1, float v2, float v3,
				   float v4, float v5, float v6, float v7)					__NE___;

		ND_ Self	operator - ()								C_NE___	{ return Negative(); }

		ND_ Self	operator +  (const Self &rhs)				C_NE___	{ return Add( rhs ); }
		ND_ Self	operator -  (const Self &rhs)				C_NE___	{ return Sub( rhs ); }
		ND_ Self	operator *  (const Self &rhs)				C_NE___	{ return Mul( rhs ); }

		ND_ Self	operator +  (float16_t rhs)					C_NE___	{ return Add( rhs ); }
		ND_ Self	operator -  (float16_t rhs)					C_NE___	{ return Sub( rhs ); }
		ND_ Self	operator *  (float16_t rhs)					C_NE___	{ return Mul( rhs ); }
		ND_ Self	operator /  (float16_t rhs)					C_NE___	{ return PreciseDiv( rhs ); }

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

		template <uint I> ND_ Scalar_t	get ()					C_NE___	{ StaticAssert( I < count );  return Scalar_t{vgetq_lane_f16( _value, I )}; }
		template <uint I> ND_ Self		set (float16_t val)		C_NE___	{ StaticAssert( I < count );  return Self{ vsetq_lane_f16( val, _value, I )}; }
		template <uint I> ND_ Self		set (Scalar_t  val)		C_NE___	{ return set<I>( float16_t{val} ); }

		ND_ Native_t&		Ref ()								__NE___	{ return _value; }
		ND_ Native_t const&	Ref ()								C_NE___	{ return _value; }

	  #if AE_SIMD_NEON64
		ND_ Scalar_t	PrefixMax ()							C_NE___	{ return Scalar_t{ vmaxvq_f16( _value )}; }
		ND_ Scalar_t	PrefixMin ()							C_NE___	{ return Scalar_t{ vminvq_f16( _value )}; }
	  #endif

		ND_ Self	Add (const Self &rhs)						C_NE___	{ return Self{ vaddq_f16( _value, rhs._value )}; }
		ND_ Self	Sub (const Self &rhs)						C_NE___	{ return Self{ vsubq_f16( _value, rhs._value )}; }
		ND_ Self	Mul (const Self &rhs)						C_NE___	{ return Self{ vmulq_f16( _value, rhs._value )}; }
		ND_ Self	Min (const Self &rhs)						C_NE___	{ return Self{ vminq_f16( _value, rhs._value )}; }
		ND_ Self	Max (const Self &rhs)						C_NE___	{ return Self{ vmaxq_f16( _value, rhs._value )}; }

		ND_ Self	Add (float16_t rhs)							C_NE___	{ return Self{ vaddq_f16( _value, vdupq_n_f16(rhs) )}; }
		ND_ Self	Sub (float16_t rhs)							C_NE___	{ return Self{ vsubq_f16( _value, vdupq_n_f16(rhs) )}; }
		ND_ Self	Mul (float16_t rhs)							C_NE___	{ return Self{ vmulq_n_f16( _value, rhs )}; }
		ND_ Self	Min (float16_t rhs)							C_NE___	{ return Self{ vminq_f16( _value, vdupq_n_f16(rhs) )}; }
		ND_ Self	Max (float16_t rhs)							C_NE___	{ return Self{ vmaxq_f16( _value, vdupq_n_f16(rhs) )}; }
		ND_ Self	PreciseDiv (float16_t rhs)					C_NE___	{ return Mul( float16_t( 1.0f / float(rhs) )); }

		ND_ Self	Add (Scalar_t rhs)							C_NE___	{ return Add( float16_t{rhs} ); }
		ND_ Self	Sub (Scalar_t rhs)							C_NE___	{ return Sub( float16_t{rhs} ); }
		ND_ Self	Mul (Scalar_t rhs)							C_NE___	{ return Mul( float16_t{rhs} ); }
		ND_ Self	Min (Scalar_t rhs)							C_NE___	{ return Min( float16_t{rhs} ); }
		ND_ Self	Max (Scalar_t rhs)							C_NE___	{ return Max( float16_t{rhs} ); }
		ND_ Self	PreciseDiv (Scalar_t rhs)					C_NE___	{ return Mul( float16_t( 1.0f / float(rhs) )); }

		ND_ Bool8	Equal    (const Self &rhs)					C_NE___	{ return Bool8{ vceqq_f16( _value, rhs._value )}; }
		ND_ Bool8	NotEqual (const Self &rhs)					C_NE___	{ return not Equal( rhs ); }
		ND_ Bool8  	Greater  (const Self &rhs)					C_NE___	{ return Bool8{ vcgtq_f16( _value, rhs._value )}; }
		ND_ Bool8  	GEqual   (const Self &rhs)					C_NE___	{ return Bool8{ vcgeq_f16( _value, rhs._value )}; }
		ND_ Bool8  	Less     (const Self &rhs)					C_NE___	{ return Bool8{ vcltq_f16( _value, rhs._value )}; }
		ND_ Bool8  	LEqual   (const Self &rhs)					C_NE___	{ return Bool8{ vcleq_f16( _value, rhs._value )}; }

		ND_ Bool8  	AbsGreater (const Self &rhs)				C_NE___	{ return Bool8{ vcageq_f16( _value, rhs._value )}; }
		ND_ Bool8  	AbsGEqual  (const Self &rhs)				C_NE___	{ return Bool8{ vcageq_f16( _value, rhs._value )}; }
		ND_ Bool8  	AbsLess    (const Self &rhs)				C_NE___	{ return Bool8{ vcaltq_f16( _value, rhs._value )}; }
		ND_ Bool8  	AbsLEqual  (const Self &rhs)				C_NE___	{ return Bool8{ vcaleq_f16( _value, rhs._value )}; }

		// compare and return 0.0 or 1.0
		ND_ Self  	EqualF    (const Self &rhs)					C_NE___	{ return Self{ Equal( rhs )}; }
		ND_ Self  	NotEqualF (const Self &rhs)					C_NE___	{ return Self{ NotEqual( rhs )}; }
		ND_ Self  	GreaterF  (const Self &rhs)					C_NE___	{ return Self{ Greater( rhs )}; }
		ND_ Self  	GEqualF   (const Self &rhs)					C_NE___	{ return Self{ GEqual( rhs )}; }
		ND_ Self  	LessF     (const Self &rhs)					C_NE___	{ return Self{ Less( rhs )}; }
		ND_ Self  	LEqualF   (const Self &rhs)					C_NE___	{ return Self{ LEqual( rhs )}; }

		ND_ Self  	AbsGreaterF (const Self &rhs)				C_NE___	{ return Self{ AbsGreater( rhs )}; }
		ND_ Self  	AbsGEqualF  (const Self &rhs)				C_NE___	{ return Self{ AbsGEqual( rhs )}; }
		ND_ Self  	AbsLessF    (const Self &rhs)				C_NE___	{ return Self{ AbsLess( rhs )}; }
		ND_ Self  	AbsLEqualF  (const Self &rhs)				C_NE___	{ return Self{ AbsLEqual( rhs )}; }

		ND_ Bool8	BitEqual (const Self&, EnabledBitCount acc)	C_NE___;

		ND_ Self	Abs ()										C_NE___	{ return Self{ vabsq_f16( _value )}; }							// abs(x)
		ND_ Self	Negative ()									C_NE___	{ return Self{ vnegq_f16( _value )}; }							// -x

		ND_ Self	Reciprocal ()								C_NE___	{ return Self{ vrecpeq_f16( _value )}; }						// approx (1 / x)
		ND_ Self	FastInvSqrt ()								C_NE___	{ return Self{ vrsqrteq_f16( _value )}; }						// approx (1 / sqrt(x))
		ND_ Self	FastSqrt ()									C_NE___	{ return Self{ vmulq_f16( _value, vrsqrteq_f16( _value ))}; }	// approx (x / sqrt(x))
		ND_ Self	FastDiv (const Self &rhs)					C_NE___	{ return Self{ vmulq_f16( _value, vrecpeq_f16( rhs._value ))}; }// approx (a / b)

	  #if AE_SIMD_NEON64
		ND_ Self	MulExt (const Self &rhs)					C_NE___	{ return Self{ vmulxq_f16( _value, rhs._value )}; }
		ND_ Self	MulExt (float16_t rhs)						C_NE___	{ return Self{ vmulxq_n_f16( _value, rhs )}; }

		ND_ Self	PreciseSqrt ()								C_NE___	{ return Self{ vsqrtq_f16( _value )}; }

		ND_ Self	PreciseDiv (const Self &rhs)				C_NE___	{ return Self{ vdivq_f16( _value, rhs._value )}; }
		ND_ Self	operator / (const Self &rhs)				C_NE___	{ return PreciseDiv( rhs ); }
	  #else
		ND_ Self	operator / (const Self &rhs)				C_NE___	{ return FastDiv( rhs ); }
	  #endif

	  #if AE_SIMD_NEON64
		ND_ Self	Trunc ()									C_NE___	{ return Self{ vrndq_f16(  _value )}; }
		ND_ Self	Floor ()									C_NE___	{ return Self{ vrndmq_f16( _value )}; }
		ND_ Self	Ceil ()										C_NE___	{ return Self{ vrndpq_f16( _value )}; }
		ND_ Self	Round ()									C_NE___	{ return Self{ vrndaq_f16( _value )}; }	// or vrndiq_f16, vrndxq_f16
		ND_ Self	RoundEven ()								C_NE___	{ return Self{ vrndnq_f16( _value )}; }
	  #endif

	  #if AE_SIMD_FMA
		ND_ friend Self  FusedMulAdd    (const Self &a, const Self &b, const Self &c)	__NE___	{ return Self{ vfmaq_f16(   c._value, a._value, b._value )}; }	// (a * b) + c
		ND_ friend Self  FusedNegMulAdd (const Self &a, const Self &b, const Self &c)	__NE___	{ return Self{ vfmsq_f16(   c._value, a._value, b._value )}; }	// c - (a * b)
	  #endif
	  #if AE_SIMD_FMA and AE_SIMD_NEON64
		ND_ friend Self  FusedMulAdd    (const Self &a, float16_t b, const Self &c)		__NE___	{ return Self{ vfmaq_n_f16( c._value, a._value, b )}; }			// (a * b) + c
		ND_ friend Self  FusedNegMulAdd (const Self &a, float16_t b, const Self &c)		__NE___	{ return Self{ vfmsq_n_f16( c._value, a._value, b )}; }			// c - (a * b)
	  #endif

		ND_ friend Self  Lerp    (const Self &x, const Self &y, const Self &factor)						__NE___	{ return x * (Self{float16_t(1.f)} - factor) + y * factor; }
		ND_ friend Self  Select  (const Bool8 &condition, const Self &ifTrue, const Self &ifFalse)		__NE___	{ return Self{ vbslq_f16( condition.Ref(), ifTrue._value, ifFalse._value )}; }
		ND_ friend Self  SelectF (const Self &x, const Self &y, const Self &ifTrue, const Self &ifFalse)__NE___	{ return Lerp( ifFalse, ifTrue, x.LessF(y) ); }


	// per bit operations //
		ND_ Self	BitInverse ()								C_NE___	{ return Self{ vreinterpretq_f16_u16( vmvnq_u16( vreinterpretq_u16_f16( _value )))}; }

		ND_ Self	And (const Self &rhs)						C_NE___	{ return Self{ vreinterpretq_f16_u16( vandq_u16( vreinterpretq_u16_f16(_value), vreinterpretq_u16_f16(rhs._value) ))}; }
		ND_ Self	Or  (const Self &rhs)						C_NE___	{ return Self{ vreinterpretq_f16_u16( vorrq_u16( vreinterpretq_u16_f16(_value), vreinterpretq_u16_f16(rhs._value) ))}; }
		ND_ Self	Xor (const Self &rhs)						C_NE___	{ return Self{ vreinterpretq_f16_u16( veorq_u16( vreinterpretq_u16_f16(_value), vreinterpretq_u16_f16(rhs._value) ))}; }
		ND_ Self	OrNot (const Self &rhs)						C_NE___	{ return Self{ vreinterpretq_f16_u16( vornq_u16( vreinterpretq_u16_f16(_value), vreinterpretq_u16_f16(rhs._value) ))}; }	// a | ~b
		ND_ Self	AndNot (const Self &rhs)					C_NE___	{ return Self{ vreinterpretq_f16_u16( vbicq_u16( vreinterpretq_u16_f16(_value), vreinterpretq_u16_f16(rhs._value) ))}; }	// ~a & b

		ND_ Self	And (const Bool8 &rhs)						C_NE___	{ return Self{ vreinterpretq_f16_u16( vandq_u16( vreinterpretq_u16_f16(_value), rhs.Ref() ))}; }
		ND_ Self	Or  (const Bool8 &rhs)						C_NE___	{ return Self{ vreinterpretq_f16_u16( vorrq_u16( vreinterpretq_u16_f16(_value), rhs.Ref() ))}; }
		ND_ Self	Xor (const Bool8 &rhs)						C_NE___	{ return Self{ vreinterpretq_f16_u16( veorq_u16( vreinterpretq_u16_f16(_value), rhs.Ref() ))}; }
		ND_ Self	OrNot (const Bool8 &rhs)					C_NE___	{ return Self{ vreinterpretq_f16_u16( vornq_u16( vreinterpretq_u16_f16(_value), rhs.Ref() ))}; }	// a | ~b
		ND_ Self	AndNot (const Bool8 &rhs)					C_NE___	{ return Self{ vreinterpretq_f16_u16( vbicq_u16( vreinterpretq_u16_f16(_value), rhs.Ref() ))}; }	// ~a & b

		ND_ Self	And (const ushort rhs)						C_NE___	{ return Self{ vreinterpretq_f16_u16( vandq_u16( vreinterpretq_u16_f16(_value), vdupq_n_u16(rhs) ))}; }
		ND_ Self	Or  (const ushort rhs)						C_NE___	{ return Self{ vreinterpretq_f16_u16( vorrq_u16( vreinterpretq_u16_f16(_value), vdupq_n_u16(rhs) ))}; }
		ND_ Self	Xor (const ushort rhs)						C_NE___	{ return Self{ vreinterpretq_f16_u16( veorq_u16( vreinterpretq_u16_f16(_value), vdupq_n_u16(rhs) ))}; }
		ND_ Self	OrNot (const ushort rhs)					C_NE___	{ return Self{ vreinterpretq_f16_u16( vornq_u16( vreinterpretq_u16_f16(_value), vdupq_n_u16(rhs) ))}; }	// a | ~b
		ND_ Self	AndNot (const ushort rhs)					C_NE___	{ return Self{ vreinterpretq_f16_u16( vbicq_u16( vreinterpretq_u16_f16(_value), vdupq_n_u16(rhs) ))}; }	// ~a & b


	// conversion //
		template <uint Idx>
		ND_ auto	Lane ()										C_NE___;

		void		ToArray (OUT float16_t* dst)				C_NE___	{ NonNull(dst);  vst1q_f16( OUT dst, _value ); }
		void		ToArray (OUT Scalar_t* dst)					C_NE___	{ ToArray( Base::Cast<float16_t>(dst) ); }
		ND_ Array_t	ToArray ()									C_NE___	{ Array_t arr;  ToArray( OUT arr.data() );  return arr; }

		template <typename DstScalar>
		ND_ auto	Convert ()									C_NE___;

		template <typename DstType>
		ND_ DstType	BitCast ()									C_NE___;

		template <uint I=0>	ND_ auto	ToFloat ()				C_NE___;
		template <uint I=0> ND_ auto	ToShort ()				C_NE___;


	// Features //
		NdCe__ static bool  Has_Arithmetic ()					{ return true; }
		NdCe__ static bool  Has_Equal ()						{ return true; }
		NdCe__ static bool  Has_Greater ()						{ return true; }
		NdCe__ static bool  Has_Rounding ()						{ return AE_SIMD_NEON64; }
		NdCe__ static bool  Has_FusedMulAdd ()					{ return AE_SIMD_FMA; }
		NdCe__ static bool  Has_MulAdd ()						{ return false; }
		NdCe__ static bool  Has_PreciseSqrt ()					{ return AE_SIMD_NEON64; }
		NdCe__ static bool  Has_PreciseInvSqrt ()				{ return false; }
		NdCe__ static bool  Has_PreciseDiv ()					{ return AE_SIMD_NEON64; }
		NdCe__ static bool  Has_ApproxReciprocal ()				{ return true; }
		NdCe__ static bool  Has_ApproxInvSqrt ()				{ return true; }
		NdCe__ static bool  Has_Trigonometry ()					{ return false; }
		NdCe__ static bool  Has_Exponential ()					{ return false; }
		NdCe__ static bool  Has_PrefixSum ()					{ return false; }
		NdCe__ static bool  Has_PrefixMinMax ()					{ return AE_SIMD_NEON64; }
		NdCe__ static bool  Has_BitEqual ()						{ return false; }
		NdCe__ static bool  Has_Swizzle ()						{ return false; }
		NdCe__ static bool  Has_Shuffle ()						{ return false; }

		template <typename DstType>	NdCe__ static bool  Has_BitCast ();
		template <typename DstType>	NdCe__ static bool  Has_Convert ();
	};

# endif // AE_SIMD_NEON_HALF
//-----------------------------------------------------------------------------



	//
	// 128 bit float (Neon)
	//
	#define AE_SIMD_SimdFloat4
	struct SimdFloat4
	{
	// types
	public:
		static constexpr uint	count	= 4;
		static constexpr uint	lanes	= 1;

		using Scalar_t		= float;
		using Self			= SimdFloat4;
		using Native_t		= float32x4_t;
		using Array_t		= StaticArray< Scalar_t, count >;
		using SimdInt_t		= SimdInt4;
		using SimdUInt_t	= SimdUInt4;
		using Mask_t		= Base::_hidden_::MSBMask< count >;

		StaticAssert( sizeof(Array_t) == sizeof(Native_t) );


		struct Bool4
		{
		private:
			uint32x4_t	_value;

		public:
			explicit Bool4 (bool val)							__NE___ : _value{ vdupq_n_u32( val ? ~0u : 0u )} {}
			explicit Bool4 (const uint32x4_t &v)				__NE___	: _value{v} {}
			explicit Bool4 (const SimdInt_t &v)					__NE___ : _value{ v.BitCast<SimdUInt_t>().Ref() } {}
			explicit Bool4 (const SimdUInt_t &v)				__NE___ : _value{ v.Ref() } {}
			Bool4 (bool v0, bool v1, bool v2, bool v3)			__NE___ : Bool4{ SimdUInt_t{ v0, v1, v2, v3 }} {}

			ND_ Bool4	operator | (const Bool4 &rhs)			C_NE___	{ return Bool4{ vorrq_u32( _value, rhs._value )}; }
			ND_ Bool4	operator & (const Bool4 &rhs)			C_NE___	{ return Bool4{ vandq_u32( _value, rhs._value )}; }
			ND_ Bool4	operator ^ (const Bool4 &rhs)			C_NE___	{ return Bool4{ veorq_u32( _value, rhs._value )}; }
			ND_ Bool4	operator ! ()							C_NE___	{ return Bool4{ vmvnq_u32( _value )}; }

			ND_ Bool4	operator == (const Bool4 &rhs)			C_NE___	{ return Bool4{ vandq_u32( _value, rhs._value )}; }
			ND_ Bool4	operator != (const Bool4 &rhs)			C_NE___	{ return Bool4{ vbicq_u32( rhs._value, _value )}; }

			template <uint I> ND_ bool	get ()					C_NE___	{ StaticAssert( I < count );  return vgetq_lane_u32( _value, I ) == UMax; }

			ND_ bool	All ()									C_NE___	{ return SimdUInt_t{ _value }.All(); }
			ND_ bool	Any ()									C_NE___	{ return SimdUInt_t{ _value }.Any(); }
			ND_ bool	None ()									C_NE___	{ return SimdUInt_t{ _value }.None(); }

			ND_ auto&	Ref ()									C_NE___	{ return _value; }

			ND_ Mask_t	ToBitfield ()							C_NE___	{ return SimdUInt_t{ _value }.ToBitfield(); }

			template <typename DstType>
			ND_ DstType	BitCast ()								C_NE___;

			ND_ explicit operator SimdInt_t ()					C_NE___	{ return SimdUInt_t{ _value }.BitCast<SimdInt_t>(); }
			ND_ explicit operator SimdUInt_t ()					C_NE___	{ return SimdUInt_t{ _value }; }

			ND_ static Bool4	True ()							__NE___	{ return Bool4{ vdupq_n_u32( UMax )}; }
			ND_ static Bool4	False ()						__NE___	{ return Bool4{ vdupq_n_u32( 0 )}; }
		};
		using Bool_t = Bool4;


	// variables
	private:
		Native_t	_value;		// float[4]

		static constexpr uint	c_One_BitMask = 0x3f800000;	// bitcast<uint>( 1.f )


	// methods
	public:
		SimdFloat4 ()											__NE___	: SimdFloat4{ 0.f } {}
		SimdFloat4 (Zero_t)										__NE___	: SimdFloat4{ 0.f } {}
		explicit SimdFloat4 (Scalar_t val)						__NE___	: _value{ vdupq_n_f32( val )} {}
		explicit SimdFloat4 (const Scalar_t* ptr)				__NE___	: _value{ vld1q_f32( GetNonNull( ptr ))} {}
		explicit SimdFloat4 (const Native_t &val)				__NE___	: _value{ val } {}
		explicit SimdFloat4 (const Bool4 &v)					__NE___ : _value{ vreinterpretq_f32_u32( vandq_u32( v.Ref(), vdupq_n_u32(c_One_BitMask) ))} {}
		SimdFloat4 (Scalar_t x, Scalar_t y, Scalar_t z, Scalar_t w)	__NE___ { Scalar_t a[]={x,y,z,w};  _value = vld1q_f32(a); }

		explicit SimdFloat4 (float32x2_t low, float32x2_t high)	__NE___ : _value{ vcombine_f32( low, high )} {}
		explicit SimdFloat4 (float32x2_t low)					__NE___ : SimdFloat4{ low, vdup_n_f32(0.f) } {}

		ND_ Self  	operator -  ()								C_NE___	{ return Negative(); }
		ND_ Self  	operator +  (const Self &rhs)				C_NE___	{ return Add( rhs ); }
		ND_ Self  	operator -  (const Self &rhs)				C_NE___	{ return Sub( rhs ); }
		ND_ Self  	operator *  (const Self &rhs)				C_NE___	{ return Mul( rhs ); }

		ND_ Self  	operator +  (Scalar_t rhs)					C_NE___	{ return Add( rhs ); }
		ND_ Self  	operator -  (Scalar_t rhs)					C_NE___	{ return Sub( rhs ); }
		ND_ Self  	operator *  (Scalar_t rhs)					C_NE___	{ return Mul( rhs ); }
		ND_ Self  	operator /  (Scalar_t rhs)					C_NE___	{ return PreciseDiv( rhs ); }

		ND_ Bool4  	operator == (const Self &rhs)				C_NE___	{ return Equal( rhs ); }
		ND_ Bool4  	operator != (const Self &rhs)				C_NE___	{ return NotEqual( rhs ); }
		ND_ Bool4  	operator >  (const Self &rhs)				C_NE___	{ return Greater( rhs ); }
		ND_ Bool4  	operator <  (const Self &rhs)				C_NE___	{ return Less( rhs ); }
		ND_ Bool4  	operator >= (const Self &rhs)				C_NE___	{ return GEqual( rhs ); }
		ND_ Bool4 	operator <= (const Self &rhs)				C_NE___	{ return LEqual( rhs ); }

		template <uint I> ND_ Scalar_t	get ()					C_NE___	{ StaticAssert( I < count );  return vgetq_lane_f32( _value, I ); }
		template <uint I> ND_ Self		set (Scalar_t val)		C_NE___	{ StaticAssert( I < count );  return Self{ vsetq_lane_f32( val, _value, I )}; }

		ND_ Native_t&		Ref ()								__NE___	{ return _value; }
		ND_ Native_t const&	Ref ()								C_NE___	{ return _value; }

	  #if AE_SIMD_NEON64
		ND_ Scalar_t	PrefixSum ()							C_NE___	{ return vaddvq_f32( _value ); }
		ND_ Scalar_t	PrefixMax ()							C_NE___	{ return vmaxvq_f32( _value ); }
		ND_ Scalar_t	PrefixMin ()							C_NE___	{ return vminvq_f32( _value ); }
	  #endif

		ND_ Self  	Add (const Self &rhs)						C_NE___	{ return Self{ vaddq_f32( _value, rhs._value )}; }
		ND_ Self  	Sub (const Self &rhs)						C_NE___	{ return Self{ vsubq_f32( _value, rhs._value )}; }
		ND_ Self  	Mul (const Self &rhs)						C_NE___	{ return Self{ vmulq_f32( _value, rhs._value )}; }
		ND_ Self  	Min (const Self &rhs)						C_NE___	{ return Self{ vminq_f32( _value, rhs._value )}; }
		ND_ Self  	Max (const Self &rhs)						C_NE___	{ return Self{ vmaxq_f32( _value, rhs._value )}; }

		ND_ Self  	Add (Scalar_t rhs)							C_NE___	{ return Self{ vaddq_f32( _value, vdupq_n_f32( rhs ))}; }
		ND_ Self  	Sub (Scalar_t rhs)							C_NE___	{ return Self{ vsubq_f32( _value, vdupq_n_f32( rhs ))}; }
		ND_ Self  	Mul (Scalar_t rhs)							C_NE___	{ return Self{ vmulq_n_f32( _value, rhs )}; }
		ND_ Self  	PreciseDiv (Scalar_t rhs)					C_NE___	{ return Mul( 1.0f / rhs ); }
		ND_ Self  	Min (Scalar_t rhs)							C_NE___	{ return Self{ vminq_f32( _value, vdupq_n_f32( rhs ))}; }
		ND_ Self  	Max (Scalar_t rhs)							C_NE___	{ return Self{ vmaxq_f32( _value, vdupq_n_f32( rhs ))}; }

		ND_ Bool4  	Equal    (const Self &rhs)					C_NE___	{ return Bool4{ vceqq_f32( _value, rhs._value )}; }
		ND_ Bool4  	NotEqual (const Self &rhs)					C_NE___	{ return not Equal( rhs ); }
		ND_ Bool4  	Greater  (const Self &rhs)					C_NE___	{ return Bool4{ vcgtq_f32( _value, rhs._value )}; }
		ND_ Bool4  	GEqual   (const Self &rhs)					C_NE___	{ return Bool4{ vcgeq_f32( _value, rhs._value )}; }
		ND_ Bool4  	Less     (const Self &rhs)					C_NE___	{ return Bool4{ vcltq_f32( _value, rhs._value )}; }
		ND_ Bool4  	LEqual   (const Self &rhs)					C_NE___	{ return Bool4{ vcleq_f32( _value, rhs._value )}; }

		ND_ Bool4  	AbsGreater (const Self &rhs)				C_NE___	{ return Bool4{ vcagtq_f32( _value, rhs._value )}; }
		ND_ Bool4  	AbsGEqual  (const Self &rhs)				C_NE___	{ return Bool4{ vcageq_f32( _value, rhs._value )}; }
		ND_ Bool4  	AbsLess    (const Self &rhs)				C_NE___	{ return Bool4{ vcaltq_f32( _value, rhs._value )}; }
		ND_ Bool4  	AbsLEqual  (const Self &rhs)				C_NE___	{ return Bool4{ vcaleq_f32( _value, rhs._value )}; }

		// compare and return 0.0 or 1.0
		ND_ Self  	EqualF    (const Self &rhs)					C_NE___	{ return Self{ Equal( rhs )}; }
		ND_ Self  	NotEqualF (const Self &rhs)					C_NE___	{ return Self{ NotEqual( rhs )}; }
		ND_ Self  	GreaterF  (const Self &rhs)					C_NE___	{ return Self{ Greater( rhs )}; }
		ND_ Self  	GEqualF   (const Self &rhs)					C_NE___	{ return Self{ GEqual( rhs )}; }
		ND_ Self  	LessF     (const Self &rhs)					C_NE___	{ return Self{ Less( rhs )}; }
		ND_ Self  	LEqualF   (const Self &rhs)					C_NE___	{ return Self{ LEqual( rhs )}; }

		ND_ Bool4	BitEqual (const Self&, EnabledBitCount acc)	C_NE___;

		ND_ Self  	Abs ()										C_NE___	{ return Self{ vabsq_f32( _value )}; }							// abs(x)
		ND_ Self  	Negative ()									C_NE___	{ return Self{ vnegq_f32( _value )}; }							// -x

		ND_ Self  	Reciprocal ()								C_NE___	{ return Self{ vrecpeq_f32( _value )}; }						// approx (1 / x)
		ND_ Self  	FastInvSqrt ()								C_NE___	{ return Self{ vrsqrteq_f32( _value )}; }						// approx (1 / sqrt(x))
		ND_ Self  	FastSqrt ()									C_NE___	{ return Self{ vmulq_f32( _value, vrsqrteq_f32( _value ))}; }	// approx (x / sqrt(x))
		ND_ Self  	FastDiv (const Self &rhs)					C_NE___	{ return Self{ vmulq_f32( _value, vrecpeq_f32( rhs._value ))}; }// approx (a / b)

	  #if AE_SIMD_NEON64
		ND_ Self	MulExt (const Self &rhs)					C_NE___	{ return Self{ vmulxq_f32( _value, rhs._value )}; }
		ND_ Self	PreciseSqrt ()								C_NE___	{ return Self{ vsqrtq_f32( _value )}; }

		ND_ Self  	PreciseDiv (const Self &rhs)				C_NE___	{ return Self{ vdivq_f32( _value, rhs._value )}; }
		ND_ Self  	operator /  (const Self &rhs)				C_NE___	{ return PreciseDiv( rhs ); }
	  #else
		ND_ Self  	operator /  (const Self &rhs)				C_NE___	{ return FastDiv( rhs ); }
	  #endif

	  #if AE_SIMD_NEON64
		ND_ Self  	Trunc ()									C_NE___	{ return Self{ vrndq_f32( _value )}; }
		ND_ Self  	Floor ()									C_NE___	{ return Self{ vrndmq_f32( _value )}; }
		ND_ Self  	Ceil ()										C_NE___	{ return Self{ vrndpq_f32( _value )}; }
		ND_ Self  	Round ()									C_NE___	{ return Self{ vrndaq_f32( _value )}; }	// or vrndiq_f32, vrndxq_f32
		ND_ Self  	RoundEven ()								C_NE___	{ return Self{ vrndnq_f32( _value )}; }
	  #endif

		ND_ friend Self  MulAdd    (const Self &a, const Self &b, const Self &c)		__NE___	{ return Self{ vmlaq_f32(   c._value, a._value, b._value )}; }	// (a * b) + c
		ND_ friend Self  MulAdd    (const Self &a, Scalar_t    b, const Self &c)		__NE___	{ return Self{ vmlaq_n_f32( c._value, a._value, b        )}; }	//
		ND_ friend Self  NegMulAdd (const Self &a, const Self &b, const Self &c)		__NE___	{ return Self{ vmlsq_f32(   c._value, a._value, b._value )}; }	// c - (a * b)
		ND_ friend Self  NegMulAdd (const Self &a, Scalar_t    b, const Self &c)		__NE___	{ return Self{ vmlsq_n_f32( c._value, a._value, b        )}; }	//

	  #if AE_SIMD_FMA
		ND_ friend Self  FusedMulAdd (const Self &a, const Self &b, const Self &c)		__NE___	{ return Self{ vfmaq_f32(   c._value, a._value, b._value )}; }	// (a * b) + c
		ND_ friend Self  FusedMulAdd (const Self &a, Scalar_t    b, const Self &c)		__NE___	{ return Self{ vfmaq_n_f32( c._value, a._value, b )}; }			//
	  #endif
	  #if AE_SIMD_FMA and AE_SIMD_NEON64
		ND_ friend Self  FusedNegMulAdd (const Self &a, const Self &b, const Self &c)	__NE___	{ return Self{ vfmsq_f32(   c._value, a._value, b._value )}; }	// c - (a * b)
		ND_ friend Self  FusedNegMulAdd (const Self &a, Scalar_t    b, const Self &c)	__NE___	{ return Self{ vfmsq_n_f32( c._value, a._value, b )}; }			//
	  #endif

		ND_ friend Self  Lerp    (const Self &x, const Self &y, const Self &factor)						__NE___	{ return x * (Self{1.f} - factor) + y * factor; }
		ND_ friend Self  Select  (const Bool4 &condition, const Self &ifTrue, const Self &ifFalse)		__NE___	{ return Self{ vbslq_f32( condition.Ref(), ifTrue._value, ifFalse._value )}; }
		ND_ friend Self  SelectF (const Self &x, const Self &y, const Self &ifTrue, const Self &ifFalse)__NE___	{ return Lerp( ifFalse, ifTrue, x.LessF(y) ); }


	// per bit operations //
		ND_ Self	BitInverse ()								C_NE___	{ return Self{ vreinterpretq_f32_u32( vmvnq_u32( vreinterpretq_u32_f32( _value )))}; }

		ND_ Self	And (const Self &rhs)						C_NE___	{ return Self{ vreinterpretq_f32_u32( vandq_u32( vreinterpretq_u32_f32(_value), vreinterpretq_u32_f32(rhs._value) ))}; }
		ND_ Self	Or  (const Self &rhs)						C_NE___	{ return Self{ vreinterpretq_f32_u32( vorrq_u32( vreinterpretq_u32_f32(_value), vreinterpretq_u32_f32(rhs._value) ))}; }
		ND_ Self	Xor (const Self &rhs)						C_NE___	{ return Self{ vreinterpretq_f32_u32( veorq_u32( vreinterpretq_u32_f32(_value), vreinterpretq_u32_f32(rhs._value) ))}; }
		ND_ Self	OrNot (const Self &rhs)						C_NE___	{ return Self{ vreinterpretq_f32_u32( vornq_u32( vreinterpretq_u32_f32(_value), vreinterpretq_u32_f32(rhs._value) ))}; }	// a | ~b
		ND_ Self	AndNot (const Self &rhs)					C_NE___	{ return Self{ vreinterpretq_f32_u32( vbicq_u32( vreinterpretq_u32_f32(_value), vreinterpretq_u32_f32(rhs._value) ))}; }	// ~a & b

		ND_ Self	And (const Bool4 &rhs)						C_NE___	{ return Self{ vreinterpretq_f32_u32( vandq_u32( vreinterpretq_u32_f32(_value), rhs.Ref() ))}; }
		ND_ Self	Or  (const Bool4 &rhs)						C_NE___	{ return Self{ vreinterpretq_f32_u32( vorrq_u32( vreinterpretq_u32_f32(_value), rhs.Ref() ))}; }
		ND_ Self	Xor (const Bool4 &rhs)						C_NE___	{ return Self{ vreinterpretq_f32_u32( veorq_u32( vreinterpretq_u32_f32(_value), rhs.Ref() ))}; }
		ND_ Self	OrNot (const Bool4 &rhs)					C_NE___	{ return Self{ vreinterpretq_f32_u32( vornq_u32( vreinterpretq_u32_f32(_value), rhs.Ref() ))}; }	// a | ~b
		ND_ Self	AndNot (const Bool4 &rhs)					C_NE___	{ return Self{ vreinterpretq_f32_u32( vbicq_u32( vreinterpretq_u32_f32(_value), rhs.Ref() ))}; }	// ~a & b

		ND_ Self	And (const uint rhs)						C_NE___	{ return Self{ vreinterpretq_f32_u32( vandq_u32( vreinterpretq_u32_f32(_value), vdupq_n_u32(rhs) ))}; }
		ND_ Self	Or  (const uint rhs)						C_NE___	{ return Self{ vreinterpretq_f32_u32( vorrq_u32( vreinterpretq_u32_f32(_value), vdupq_n_u32(rhs) ))}; }
		ND_ Self	Xor (const uint rhs)						C_NE___	{ return Self{ vreinterpretq_f32_u32( veorq_u32( vreinterpretq_u32_f32(_value), vdupq_n_u32(rhs) ))}; }
		ND_ Self	OrNot (const uint rhs)						C_NE___	{ return Self{ vreinterpretq_f32_u32( vornq_u32( vreinterpretq_u32_f32(_value), vdupq_n_u32(rhs) ))}; }	// a | ~b
		ND_ Self	AndNot (const uint rhs)						C_NE___	{ return Self{ vreinterpretq_f32_u32( vbicq_u32( vreinterpretq_u32_f32(_value), vdupq_n_u32(rhs) ))}; }	// ~a & b


	// conversion //
		template <uint Idx>
		ND_ Self	Lane ()										C_NE___ { StaticAssert( Idx < lanes );  return *this; }

		template <uint X, uint Y, uint Z, uint W>
		ND_ Self	Swizzle ()									C_NE___;

		template <uint AX, uint AY, uint BZ, uint BW>
		ND_ Self	Shuffle (const Self &b)						C_NE___;

		ND_ Array_t	ToArray ()									C_NE___	{ Array_t arr;  vst1q_f32( OUT arr.data(), _value );  return arr; }
			void	ToArray (OUT Scalar_t* dst)					C_NE___	{ NonNull( dst );  vst1q_f32( OUT dst, _value ); }

		ND_ explicit operator packed_float4 ()					C_NE___	{ packed_float4 tmp;  vst1q_f32( OUT &tmp.x, _value );  return tmp; }

	  #ifdef AE_SIMD_SimdHalf4
		ND_ explicit operator SimdHalf4 ()						C_NE___	{ return SimdHalf4{ vcvt_f16_f32( _value )}; }
	  #endif

		template <typename DstScalar>
		ND_ auto	Convert ()									C_NE___;

		template <typename DstType>
		ND_ DstType	BitCast ()									C_NE___;

		template <uint I=0>
		ND_ SimdDouble2	ToDouble ()								C_NE___;
		ND_ SimdInt4	ToInt ()								C_NE___;
		ND_ SimdHalf4	ToHalf ()								C_NE___;


	// Features //
		NdCe__ static bool  Has_Arithmetic ()					{ return true; }
		NdCe__ static bool  Has_Equal ()						{ return true; }
		NdCe__ static bool  Has_Greater ()						{ return true; }
		NdCe__ static bool  Has_Rounding ()						{ return AE_SIMD_NEON64 > 0; }
		NdCe__ static bool  Has_PreciseDiv ()					{ return AE_SIMD_NEON64 > 0; }
		NdCe__ static bool  Has_FusedMulAdd ()					{ return AE_SIMD_FMA > 0; }
		NdCe__ static bool  Has_MulAdd ()						{ return true; }
		NdCe__ static bool  Has_PreciseSqrt ()					{ return AE_SIMD_NEON64 > 0; }
		NdCe__ static bool  Has_PreciseInvSqrt ()				{ return false; }
		NdCe__ static bool  Has_ApproxReciprocal ()				{ return true; }
		NdCe__ static bool  Has_ApproxInvSqrt ()				{ return true; }
		NdCe__ static bool  Has_Trigonometry ()					{ return false; }
		NdCe__ static bool  Has_Exponential ()					{ return false; }
		NdCe__ static bool  Has_PrefixSum ()					{ return false; }
		NdCe__ static bool  Has_PrefixMinMax ()					{ return false; }
		NdCe__ static bool  Has_BitEqual ()						{ return false; }
		NdCe__ static bool  Has_Swizzle ()						{ return true; }
		NdCe__ static bool  Has_Shuffle ()						{ return true; }

		template <typename DstType>	NdCe__ static bool  Has_BitCast ();
		template <typename DstType>	NdCe__ static bool  Has_Convert ();
	};
//-----------------------------------------------------------------------------



#if AE_SIMD_NEON64

	//
	// 128 bit double (Neon64)
	//
	#define AE_SIMD_SimdDouble2
	struct SimdDouble2
	{
	// types
	public:
		static constexpr uint	count	= 2;
		static constexpr uint	lanes	= 1;

		using Scalar_t		= double;
		using Self			= SimdDouble2;
		using Native_t		= float64x2_t;
		using Array_t		= StaticArray< Scalar_t, count >;
		using SimdInt_t		= SimdLong2;
		using SimdUInt_t	= SimdULong2;
		using Mask_t		= Base::_hidden_::MSBMask< count >;

		StaticAssert( sizeof(Array_t) == sizeof(Native_t) );


		struct Bool2
		{
		private:
			uint64x2_t	_value;

		public:
			explicit Bool2 (bool val)							__NE___ : _value{ vdupq_n_u64( val ? ~0ull : 0ull )} {}
			explicit Bool2 (const uint64x2_t &v)				__NE___	: _value{v} {}
			explicit Bool2 (const SimdInt_t &v)					__NE___ : _value{ v.BitCast<SimdUInt_t>().Ref() } {}
			explicit Bool2 (const SimdUInt_t &v)				__NE___ : _value{ v.Ref() } {}
			Bool2 (bool v0, bool v1)							__NE___ : Bool2{ SimdUInt_t{ v0, v1 }} {}

			ND_ Bool2	operator | (const Bool2 &rhs)			C_NE___	{ return Bool2{ vorrq_u64( _value, rhs._value )}; }
			ND_ Bool2	operator & (const Bool2 &rhs)			C_NE___	{ return Bool2{ vandq_u64( _value, rhs._value )}; }
			ND_ Bool2	operator ^ (const Bool2 &rhs)			C_NE___	{ return Bool2{ veorq_u64( _value, rhs._value )}; }
			ND_ Bool2	operator ! ()							C_NE___	{ return Bool2{ vreinterpretq_u64_u32( vmvnq_u32( vreinterpretq_u32_u64(_value) ))}; }

			ND_ Bool2	operator == (const Bool2 &rhs)			C_NE___	{ return Bool2{ vandq_u64( _value, rhs._value )}; }
			ND_ Bool2	operator != (const Bool2 &rhs)			C_NE___	{ return Bool2{ vbicq_u64( rhs._value, _value )}; }

			template <uint I> ND_ bool	get ()               	C_NE___	{ StaticAssert( I < count );  return vgetq_lane_u64( _value, I ) == UMax; }

			ND_ bool	All ()									C_NE___	{ return SimdUInt_t{ _value }.All(); }
			ND_ bool	Any ()									C_NE___	{ return SimdUInt_t{ _value }.Any(); }
			ND_ bool	None ()									C_NE___	{ return SimdUInt_t{ _value }.None(); }

			ND_ auto&	Ref ()									C_NE___	{ return _value; }

			ND_ Mask_t	ToBitfield ()							C_NE___	{ return SimdUInt_t{ _value }.ToBitfield(); }

			template <typename DstType>
			ND_ DstType	BitCast ()								C_NE___;

			ND_ explicit operator SimdInt_t ()					C_NE___	{ return SimdUInt_t{ _value }.BitCast<SimdInt_t>(); }
			ND_ explicit operator SimdUInt_t ()					C_NE___	{ return SimdUInt_t{ _value }; }

			ND_ static Bool2	True ()							__NE___	{ return Bool2{ vdupq_n_u64( UMax )}; }
			ND_ static Bool2	False ()						__NE___	{ return Bool2{ vdupq_n_u64( 0 )}; }
		};
		using Bool_t = Bool2;


	// variables
	private:
		Native_t	_value;		// double[2]

		static constexpr ulong	c_One_BitMask = 0x3ff0000000000000;	// bitcast<ulong>( 1.0 )


	// methods
	public:
		SimdDouble2 ()											__NE___	: SimdDouble2{ 0.0 } {}
		SimdDouble2 (Zero_t)									__NE___ : SimdDouble2{ 0.0 } {}
		explicit SimdDouble2 (Scalar_t val)						__NE___	: _value{ vdupq_n_f64( val )} {}
		explicit SimdDouble2 (const Scalar_t* ptr)				__NE___	: _value{ vld1q_f64( GetNonNull( ptr ))} {}
		explicit SimdDouble2 (const Native_t &val)				__NE___	: _value{ val } {}
		explicit SimdDouble2 (const Bool2 &v)					__NE___ : _value{ vreinterpretq_f64_u64( vandq_u64( v.Ref(), vdupq_n_u64(c_One_BitMask) ))} {}
		SimdDouble2 (Scalar_t x, Scalar_t y)					__NE___ { double a[]={x,y};  _value = vld1q_f64(a); }

		ND_ Self	operator +  (const Self &rhs)				C_NE___	{ return Add( rhs ); }
		ND_ Self	operator -  (const Self &rhs)				C_NE___	{ return Sub( rhs ); }
		ND_ Self	operator *  (const Self &rhs)				C_NE___	{ return Mul( rhs ); }
		ND_ Self	operator /  (const Self &rhs)				C_NE___	{ return PreciseDiv( rhs ); }

		ND_ Self	operator +  (Scalar_t rhs)					C_NE___	{ return Add( rhs ); }
		ND_ Self	operator -  (Scalar_t rhs)					C_NE___	{ return Sub( rhs ); }
		ND_ Self	operator *  (Scalar_t rhs)					C_NE___	{ return Mul( rhs ); }
		ND_ Self	operator /  (Scalar_t rhs)					C_NE___	{ return PreciseDiv( rhs ); }

		ND_ Bool2	operator == (const Self &rhs)				C_NE___	{ return Equal( rhs ); }
		ND_ Bool2	operator != (const Self &rhs)				C_NE___	{ return NotEqual( rhs ); }
		ND_ Bool2	operator >  (const Self &rhs)				C_NE___	{ return Greater( rhs ); }
		ND_ Bool2	operator <  (const Self &rhs)				C_NE___	{ return Less( rhs ); }
		ND_ Bool2	operator >= (const Self &rhs)				C_NE___	{ return GEqual( rhs ); }
		ND_ Bool2	operator <= (const Self &rhs)				C_NE___	{ return LEqual( rhs ); }

		template <uint I> ND_ Scalar_t	get ()              	C_NE___	{ StaticAssert( I < count );  return vgetq_lane_f64( _value, I ); }
		template <uint I> ND_ Self		set (Scalar_t val)		C_NE___ { StaticAssert( I < count );  return Self{ vsetq_lane_f64( val, _value, I )}; }

		ND_ Native_t&		Ref ()								__NE___	{ return _value; }
		ND_ Native_t const&	Ref ()								C_NE___	{ return _value; }

		ND_ Scalar_t	PrefixSum ()							C_NE___	{ return vaddvq_f64( _value ); }
		ND_ Scalar_t	PrefixMax ()							C_NE___	{ return vmaxvq_f64( _value ); }
		ND_ Scalar_t	PrefixMin ()							C_NE___	{ return vminvq_f64( _value ); }

		ND_ Self	Add (const Self &rhs)						C_NE___	{ return Self{ vaddq_f64( _value, rhs._value )}; }
		ND_ Self	Sub (const Self &rhs)						C_NE___	{ return Self{ vsubq_f64( _value, rhs._value )}; }
		ND_ Self	Mul (const Self &rhs)						C_NE___	{ return Self{ vmulq_f64( _value, rhs._value )}; }
		ND_ Self	PreciseDiv (const Self &rhs)				C_NE___	{ return Self{ vdivq_f64( _value, rhs._value )}; }
		ND_ Self	Min (const Self &rhs)						C_NE___	{ return Self{ vminq_f64( _value, rhs._value )}; }
		ND_ Self	Max (const Self &rhs)						C_NE___	{ return Self{ vmaxq_f64( _value, rhs._value )}; }

		ND_ Self	Add (Scalar_t rhs)							C_NE___	{ return Self{ vaddq_f64( _value, vdupq_n_f64(rhs) )}; }
		ND_ Self	Sub (Scalar_t rhs)							C_NE___	{ return Self{ vsubq_f64( _value, vdupq_n_f64(rhs) )}; }
		ND_ Self	Mul (Scalar_t rhs)							C_NE___	{ return Self{ vmulq_n_f64( _value, rhs )}; }
		ND_ Self	PreciseDiv (Scalar_t rhs)					C_NE___	{ return Self{ vdivq_f64( _value, vdupq_n_f64(rhs) )}; }
		ND_ Self	Min (Scalar_t rhs)							C_NE___	{ return Self{ vminq_f64( _value, vdupq_n_f64(rhs) )}; }
		ND_ Self	Max (Scalar_t rhs)							C_NE___	{ return Self{ vmaxq_f64( _value, vdupq_n_f64(rhs) )}; }

		ND_ Bool2	Equal    (const Self &rhs)					C_NE___	{ return Bool2{ vceqq_f64( _value, rhs._value )}; }
		ND_ Bool2	NotEqual (const Self &rhs)					C_NE___	{ return not Equal( rhs ); }
		ND_ Bool2  	Greater  (const Self &rhs)					C_NE___	{ return Bool2{ vcgtq_f64( _value, rhs._value )}; }
		ND_ Bool2  	GEqual   (const Self &rhs)					C_NE___	{ return Bool2{ vcgeq_f64( _value, rhs._value )}; }
		ND_ Bool2  	Less     (const Self &rhs)					C_NE___	{ return Bool2{ vcltq_f64( _value, rhs._value )}; }
		ND_ Bool2  	LEqual   (const Self &rhs)					C_NE___	{ return Bool2{ vcleq_f64( _value, rhs._value )}; }

		ND_ Bool2  	AbsGreater (const Self &rhs)				C_NE___	{ return Bool2{ vcagtq_f64( _value, rhs._value )}; }
		ND_ Bool2  	AbsGEqual  (const Self &rhs)				C_NE___	{ return Bool2{ vcageq_f64( _value, rhs._value )}; }
		ND_ Bool2  	AbsLess    (const Self &rhs)				C_NE___	{ return Bool2{ vcaltq_f64( _value, rhs._value )}; }
		ND_ Bool2  	AbsLEqual  (const Self &rhs)				C_NE___	{ return Bool2{ vcaleq_f64( _value, rhs._value )}; }

		// compare and return 0.0 or 1.0
		ND_ Self  	EqualF    (const Self &rhs)					C_NE___	{ return Self{ Equal( rhs )}; }
		ND_ Self  	NotEqualF (const Self &rhs)					C_NE___	{ return Self{ NotEqual( rhs )}; }
		ND_ Self  	GreaterF  (const Self &rhs)					C_NE___	{ return Self{ Greater( rhs )}; }
		ND_ Self  	GEqualF   (const Self &rhs)					C_NE___	{ return Self{ GEqual( rhs )}; }
		ND_ Self  	LessF     (const Self &rhs)					C_NE___	{ return Self{ Less( rhs )}; }
		ND_ Self  	LEqualF   (const Self &rhs)					C_NE___	{ return Self{ LEqual( rhs )}; }

		ND_ Bool2	BitEqual (const Self&, EnabledBitCount acc)	C_NE___;

		ND_ Self	Abs ()										C_NE___	{ return Self{ vabsq_f64( _value )}; }							// abs(x)
		ND_ Self	Negative ()									C_NE___	{ return Self{ vnegq_f64( _value )}; }							// -x

		ND_ Self	Reciprocal ()								C_NE___	{ return Self{ vrecpeq_f64( _value )}; }						// approx (1 / x)
		ND_ Self	FastInvSqrt ()								C_NE___	{ return Self{ vrsqrteq_f64( _value )}; }						// approx (1 / sqrt(x))
		ND_ Self	FastSqrt ()									C_NE___	{ return Self{ vmulq_f64( _value, vrsqrteq_f64( _value ))}; }	// approx (x / sqrt(x))
		ND_ Self  	FastDiv (const Self &rhs)					C_NE___	{ return Self{ vmulq_f64( _value, vrecpeq_f64( rhs._value ))}; }// approx (a / b)

		ND_ Self	MulExt (const Self &rhs)					C_NE___	{ return Self{ vmulxq_f64( _value, rhs._value )}; }

		ND_ Self	PreciseSqrt ()								C_NE___	{ return Self{ vsqrtq_f64( _value )}; }

		ND_ Self	Trunc ()									C_NE___	{ return Self{ vrndq_f64( _value )}; }
		ND_ Self	Floor ()									C_NE___	{ return Self{ vrndmq_f64( _value )}; }
		ND_ Self	Ceil ()										C_NE___	{ return Self{ vrndpq_f64( _value )}; }
		ND_ Self	Round ()									C_NE___	{ return Self{ vrndaq_f64( _value )}; }	// or vrndiq_f64, vrndxq_f64
		ND_ Self	RoundEven ()								C_NE___	{ return Self{ vrndnq_f64( _value )}; }

		ND_ friend Self  MulAdd    (const Self &a, const Self &b, const Self &c)		__NE___	{ return Self{ vmlaq_f64(   c._value, a._value, b._value )}; }	// (a * b) + c
		ND_ friend Self  NegMulAdd (const Self &a, const Self &b, const Self &c)		__NE___	{ return Self{ vmlsq_f64(   c._value, a._value, b._value )}; }	// c - (a * b)

	  #if AE_SIMD_FMA
		ND_ friend Self  FusedMulAdd    (const Self &a, const Self &b, const Self &c)	__NE___	{ return Self{ vfmaq_f64(   c._value, a._value, b._value )}; }	// (a * b) + c
		ND_ friend Self  FusedMulAdd    (const Self &a, Scalar_t    b, const Self &c)	__NE___	{ return Self{ vfmaq_n_f64( c._value, a._value, b )}; }			//
		ND_ friend Self  FusedNegMulAdd (const Self &a, const Self &b, const Self &c)	__NE___	{ return Self{ vfmsq_f64(   c._value, a._value, b._value )}; }	// c - (a * b)
		ND_ friend Self  FusedNegMulAdd (const Self &a, Scalar_t    b, const Self &c)	__NE___	{ return Self{ vfmsq_n_f64( c._value, a._value, b )}; }			//
	  #endif

		ND_ friend Self  Lerp    (const Self &x, const Self &y, const Self &factor)						__NE___	{ return x * (Self{1.0} - factor) + y * factor; }
		ND_ friend Self  Select  (const Bool2 &condition, const Self &ifTrue, const Self &ifFalse)		__NE___	{ return Self{ vbslq_f64( condition.Ref(), ifTrue._value, ifFalse._value )}; }
		ND_ friend Self  SelectF (const Self &x, const Self &y, const Self &ifTrue, const Self &ifFalse)__NE___	{ return Lerp( ifFalse, ifTrue, x.LessF(y) ); }


	// per bit operations //
		ND_ Self	BitInverse ()								C_NE___	{ return Self{ vreinterpretq_f64_u64( vmvnq_u32( vreinterpretq_u64_f64( _value )))}; }

		ND_ Self	And (const Self &rhs)						C_NE___	{ return Self{ vreinterpretq_f64_u64( vandq_u32( vreinterpretq_u64_f64(_value), vreinterpretq_u64_f64(rhs._value) ))}; }
		ND_ Self	Or  (const Self &rhs)						C_NE___	{ return Self{ vreinterpretq_f64_u64( vorrq_u32( vreinterpretq_u64_f64(_value), vreinterpretq_u64_f64(rhs._value) ))}; }
		ND_ Self	Xor (const Self &rhs)						C_NE___	{ return Self{ vreinterpretq_f64_u64( veorq_u32( vreinterpretq_u64_f64(_value), vreinterpretq_u64_f64(rhs._value) ))}; }
		ND_ Self	OrNot (const Self &rhs)						C_NE___	{ return Self{ vreinterpretq_f64_u64( vornq_u32( vreinterpretq_u64_f64(_value), vreinterpretq_u64_f64(rhs._value) ))}; }	// a | ~b
		ND_ Self	AndNot (const Self &rhs)					C_NE___	{ return Self{ vreinterpretq_f64_u64( vbicq_u32( vreinterpretq_u64_f64(_value), vreinterpretq_u64_f64(rhs._value) ))}; }	// ~a & b

		ND_ Self	And (const Bool2 &rhs)						C_NE___	{ return Self{ vreinterpretq_f64_u64( vandq_u32( vreinterpretq_u64_f64(_value), rhs.Ref() ))}; }
		ND_ Self	Or  (const Bool2 &rhs)						C_NE___	{ return Self{ vreinterpretq_f64_u64( vorrq_u32( vreinterpretq_u64_f64(_value), rhs.Ref() ))}; }
		ND_ Self	Xor (const Bool2 &rhs)						C_NE___	{ return Self{ vreinterpretq_f64_u64( veorq_u32( vreinterpretq_u64_f64(_value), rhs.Ref() ))}; }
		ND_ Self	OrNot (const Bool2 &rhs)					C_NE___	{ return Self{ vreinterpretq_f64_u64( vornq_u32( vreinterpretq_u64_f64(_value), rhs.Ref() ))}; }	// a | ~b
		ND_ Self	AndNot (const Bool2 &rhs)					C_NE___	{ return Self{ vreinterpretq_f64_u64( vbicq_u32( vreinterpretq_u64_f64(_value), rhs.Ref() ))}; }	// ~a & b

		ND_ Self	And (const ulong rhs)						C_NE___	{ return Self{ vreinterpretq_f64_u64( vandq_u32( vreinterpretq_u64_f64(_value), vdupq_n_u64(rhs) ))}; }
		ND_ Self	Or  (const ulong rhs)						C_NE___	{ return Self{ vreinterpretq_f64_u64( vorrq_u32( vreinterpretq_u64_f64(_value), vdupq_n_u64(rhs) ))}; }
		ND_ Self	Xor (const ulong rhs)						C_NE___	{ return Self{ vreinterpretq_f64_u64( veorq_u32( vreinterpretq_u64_f64(_value), vdupq_n_u64(rhs) ))}; }
		ND_ Self	OrNot (const ulong rhs)						C_NE___	{ return Self{ vreinterpretq_f64_u64( vornq_u32( vreinterpretq_u64_f64(_value), vdupq_n_u64(rhs) ))}; }	// a | ~b
		ND_ Self	AndNot (const ulong rhs)					C_NE___	{ return Self{ vreinterpretq_f64_u64( vbicq_u32( vreinterpretq_u64_f64(_value), vdupq_n_u64(rhs) ))}; }	// ~a & b


	// conversion //
		template <uint Idx>
		ND_ Self	Lane ()										C_NE___ { StaticAssert( Idx < lanes );  return *this; }

		template <uint X, uint Y>
		ND_ Self	Swizzle ()									C_NE___;

		template <uint AX, uint BY>
		ND_ Self	Shuffle (const Self &b)						C_NE___;

		ND_ Array_t	ToArray ()									C_NE___	{ Array_t arr;  vst1q_f64( OUT arr.data(), _value );  return arr; }
			void	ToArray (OUT Scalar_t* dst)					C_NE___	{ NonNull( dst );  vst1q_f64( OUT dst, _value ); }

		template <typename DstScalar>
		ND_ auto	Convert ()									C_NE___;

		template <typename DstType>
		ND_ DstType	BitCast ()									C_NE___;

		ND_ explicit operator packed_double2 ()					C_NE___	{ packed_double2 tmp;  vst1q_f64( OUT &tmp.x, _value );  return tmp; }

		ND_ SimdFloat4	ToFloat ()								C_NE___;
		ND_ SimdInt2	ToInt ()								C_NE___;
		ND_ SimdLong2	ToLong ()								C_NE___;


	// Features //
		NdCe__ static bool  Has_Arithmetic ()					{ return true; }
		NdCe__ static bool  Has_Equal ()						{ return true; }
		NdCe__ static bool  Has_Greater ()						{ return true; }
		NdCe__ static bool  Has_Rounding ()						{ return true; }
		NdCe__ static bool  Has_FusedMulAdd ()					{ return AE_SIMD_FMA > 0; }
		NdCe__ static bool  Has_MulAdd ()						{ return true; }
		NdCe__ static bool  Has_PreciseDiv ()					{ return true; }
		NdCe__ static bool  Has_PreciseSqrt ()					{ return true; }
		NdCe__ static bool  Has_PreciseInvSqrt ()				{ return false; }
		NdCe__ static bool  Has_ApproxReciprocal ()				{ return true; }
		NdCe__ static bool  Has_ApproxInvSqrt ()				{ return true; }
		NdCe__ static bool  Has_Trigonometry ()					{ return false; }
		NdCe__ static bool  Has_Exponential ()					{ return false; }
		NdCe__ static bool  Has_PrefixSum ()					{ return false; }
		NdCe__ static bool  Has_PrefixMinMax ()					{ return false; }
		NdCe__ static bool  Has_BitEqual ()						{ return false; }
		NdCe__ static bool  Has_Swizzle ()						{ return true; }
		NdCe__ static bool  Has_Shuffle ()						{ return true; }

		template <typename DstType>	NdCe__ static bool  Has_BitCast ();
		template <typename DstType>	NdCe__ static bool  Has_Convert ();
	};

#endif // AE_SIMD_NEON64
//-----------------------------------------------------------------------------



	//
	// 128 bit int
	//
	#define AE_SIMD_Int128b
	struct Int128b
	{
	// types
	public:
		using Self	    = Int128b;
		using Native_t  = uint8x16_t;


	// variables
	private:
		Native_t	_value;


	// methods
	public:
		Int128b ()									__NE___	: _value{ vdupq_n_u8( 0 )} {}
		Int128b (Zero_t)							__NE___	: _value{ vdupq_n_u8( 0 )} {}
		Int128b (UMax_t)							__NE___	: _value{ vdupq_n_u8( UMax )} {}
		explicit Int128b (int v)					__NE___;
		explicit Int128b (slong v)					__NE___;
		explicit Int128b (const ubyte* ptr)			__NE___	: _value{ vld1q_u8( GetNonNull( ptr ))} {}
		explicit Int128b (const Native_t &val)	    __NE___	: _value{ val } {}

		ND_ Native_t &		Ref ()					__NE___	{ return _value; }
		ND_ Native_t const&	Ref ()					C_NE___	{ return _value; }


	// per bit operations //
		ND_ Self	operator ~ ()					C_NE___	{ return BitInverse(); }

		ND_ Self	operator & (const Self &rhs)	C_NE___	{ return And( rhs ); }
		ND_ Self	operator | (const Self &rhs)	C_NE___	{ return Or( rhs ); }
		ND_ Self	operator ^ (const Self &rhs)	C_NE___	{ return Xor( rhs ); }

		ND_ Self	BitInverse ()					C_NE___	{ return Self{ vmvnq_u8( _value )}; }				// ~a

		ND_ Self	And   (const Self &rhs)			C_NE___	{ return Self{ vandq_u8( _value, rhs._value )}; }	// a & b
		ND_ Self	Or    (const Self &rhs)			C_NE___	{ return Self{ vorrq_u8( _value, rhs._value )}; }	// a | b
		ND_ Self	Xor   (const Self &rhs)			C_NE___	{ return Self{ veorq_u8( _value, rhs._value )}; }	// a ^ b
		ND_ Self	OrNot (const Self &rhs)			C_NE___	{ return Self{ vornq_u8( _value, rhs._value )}; }	// a | ~b
		ND_ Self	AndNot (const Self &rhs)		C_NE___	{ return Self{ vornq_u8( _value, rhs._value )}; }	// ~a & b


	// conversion //
		template <typename T>
		ND_ auto	ToArray ()						C_NE___;
	};
//-----------------------------------------------------------------------------


} // AE::Base

#include "Neon.inl.h"

# undef AE_SIMD_NEON64
#endif // AE_SIMD_NEON
