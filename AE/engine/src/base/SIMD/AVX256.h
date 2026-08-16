// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	256 bit SIMD
*/

#pragma once

#if AE_SIMD_AVX >= 1
namespace AE::Base
{

	//
	// 256 bits float (AVX)
	//
	#define AE_SIMD_SimdFloat8
	struct SimdFloat8
	{
	// types
	public:
		static constexpr uint	count	= 8;
		static constexpr uint	parts	= 2;	// number of SSE vectors, cross lane operations may have additional cost

		using Scalar_t		= float;
		using Self			= SimdFloat8;
		using Native_t		= __m256;
		using Array_t		= StaticArray< Scalar_t, count >;
		using Ptr_t			= AlignedPtr< sizeof(Native_t) >;
		using CPtr_t		= AlignedPtr< sizeof(Native_t), true >;
		using SimdInt_t		= SimdInt8;
		using SimdUInt_t	= SimdUInt8;
		using Mask_t		= MSBMask< count, 0 >;
		StaticAssert( sizeof(Array_t) == sizeof(Native_t) );


		struct Bool8
		{
		private:
			Native_t	_value;		// uint[8]

		public:
			explicit Bool8 (bool val)							__NE___	: _value{ val ? True()._value : False()._value } {}
			explicit Bool8 (const Native_t &val)				__NE___	: _value{ val } {}
			explicit Bool8 (const __m256i &val)					__NE___	: _value{ _mm256_castsi256_ps( val )} {}
			explicit Bool8 (const SimdInt_t &v)					__NE___;
			explicit Bool8 (const SimdUInt_t &v)				__NE___;
			Bool8 (bool v0, bool v1, bool v2, bool v3,
				   bool v4, bool v5, bool v6, bool v7)			__NE___;

			ND_ Bool8	operator | (const Bool8 &rhs)			C_NE___	{ return Bool8{ _mm256_or_ps(  _value, rhs._value )}; }
			ND_ Bool8	operator & (const Bool8 &rhs)			C_NE___	{ return Bool8{ _mm256_and_ps( _value, rhs._value )}; }
			ND_ Bool8	operator ^ (const Bool8 &rhs)			C_NE___	{ return Bool8{ _mm256_xor_ps( _value, rhs._value )}; }
			ND_ Bool8	operator ! ()							C_NE___	{ return Bool8{ _mm256_andnot_ps( _value, True()._value )}; }

			ND_ Bool8	operator == (const Bool8 &rhs)			C_NE___	{ return Bool8{ _mm256_and_ps( _value, rhs._value )}; }
			ND_ Bool8	operator != (const Bool8 &rhs)			C_NE___	{ return Bool8{ _mm256_andnot_ps( _value, rhs._value )}; }

			ND_ Bool8	AndNot (const Bool8 &rhs)				C_NE___	{ return Bool8{ _mm256_andnot_ps( _value, rhs._value )}; }		// ~a & b
			ND_ Self	AndNot (const Self &rhs)				C_NE___	{ return Self{ _mm256_andnot_ps( _value, rhs.Ref() )}; }		// ~a & b

			ND_ bool	All ()									C_NE___	{ return _mm256_movemask_ps( _value ) == 0xFF; }
			ND_ bool	Any ()									C_NE___	{ return _mm256_movemask_ps( _value ) != 0; }
			ND_ bool	None ()									C_NE___	{ return _mm256_movemask_ps( _value ) == 0; }

			ND_ auto&	Ref ()									C_NE___	{ return _value; }

			ND_ Mask_t	ToBitfield ()							C_NE___	{ return Mask_t{_mm256_movemask_ps( _value )}; }

			template <typename DstType>
			ND_ DstType	BitCast ()								C_NE___;

			ND_ static Bool8	True ()							__NE___	{ auto z = _mm256_setzero_ps();  return Bool8{_mm256_cmp_ps( z, z, _CMP_EQ_OQ )}; }
			ND_ static Bool8	False ()						__NE___	{ return Bool8{_mm256_setzero_ps()}; }
		};
		using Bool_t = Bool8;


	// variables
	private:
		Native_t		_value;		// float[8]


	// methods
	public:
		SimdFloat8 ()											__NE___	: _value{ _mm256_setzero_ps() } {}
		SimdFloat8 (Zero_t)										__NE___	: _value{ _mm256_setzero_ps() } {}
		explicit SimdFloat8 (Scalar_t v)						__NE___	: _value{ _mm256_set1_ps( v )} {}
		explicit SimdFloat8 (const Native_t &v)					__NE___	: _value{ v } {}
		explicit SimdFloat8 (const Scalar_t* ptr)				__NE___	: _value{ _mm256_loadu_ps( GetNonNull( ptr ))} {}
		explicit SimdFloat8 (const CPtr_t ptr)					__NE___	: _value{ _mm256_load_ps( ptr.Cast<Scalar_t>() )} {}

		explicit SimdFloat8 (const SimdFloat4 &low)				__NE___;
		SimdFloat8 (const SimdFloat4 &low, const SimdFloat4 &high) __NE___;

		SimdFloat8 (Scalar_t v0, Scalar_t v1, Scalar_t v2, Scalar_t v3,
					Scalar_t v4, Scalar_t v5, Scalar_t v6, Scalar_t v7) __NE___ :
			_value{ _mm256_set_ps( v7, v6, v5, v4, v3, v2, v1, v0 )} {}

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

		ND_ Self	Add (const Self &rhs)						C_NE___	{ return Self{ _mm256_add_ps( _value, rhs._value )}; }
		ND_ Self	Sub (const Self &rhs)						C_NE___	{ return Self{ _mm256_sub_ps( _value, rhs._value )}; }
		ND_ Self	Mul (const Self &rhs)						C_NE___	{ return Self{ _mm256_mul_ps( _value, rhs._value )}; }
		ND_ Self	PreciseDiv (const Self &rhs)				C_NE___	{ return Self{ _mm256_div_ps( _value, rhs._value )}; }
		ND_ Self	Min (const Self &rhs)						C_NE___	{ return Self{ _mm256_min_ps( _value, rhs._value )}; }
		ND_ Self	Max (const Self &rhs)						C_NE___	{ return Self{ _mm256_max_ps( _value, rhs._value )}; }

		ND_ Self	Add (Scalar_t rhs)							C_NE___	{ return Self{ _mm256_add_ps( _value, _mm256_set1_ps(rhs) )}; }
		ND_ Self	Sub (Scalar_t rhs)							C_NE___	{ return Self{ _mm256_sub_ps( _value, _mm256_set1_ps(rhs) )}; }
		ND_ Self	Mul (Scalar_t rhs)							C_NE___	{ return Self{ _mm256_mul_ps( _value, _mm256_set1_ps(rhs) )}; }
		ND_ Self	PreciseDiv (Scalar_t rhs)					C_NE___	{ return Self{ _mm256_div_ps( _value, _mm256_set1_ps(rhs) )}; }
		ND_ Self	Min (Scalar_t rhs)							C_NE___	{ return Self{ _mm256_min_ps( _value, _mm256_set1_ps(rhs) )}; }
		ND_ Self	Max (Scalar_t rhs)							C_NE___	{ return Self{ _mm256_max_ps( _value, _mm256_set1_ps(rhs) )}; }

		ND_ Self	Abs ()										C_NE___	{ return Self{ _mm256_andnot_ps( _mm256_set1_ps( -0.0f ), _value )}; }
		ND_ Self	PreciseSqrt ()								C_NE___	{ return Self{ _mm256_sqrt_ps( _value )}; }

		ND_ Self	Reciprocal ()								C_NE___	{ return Self{ _mm256_rcp_ps( _value )}; }								// approx (1 / x)
		ND_ Self	FastInvSqrt ()								C_NE___	{ return Self{ _mm256_rsqrt_ps( _value )}; }							// approx (1 / sqrt(x))
		ND_ Self	FastSqrt ()									C_NE___	{ return Self{ _mm256_mul_ps( _value, _mm256_rsqrt_ps( _value ))}; }	// approx (x / sqrt(x))
		ND_ Self	FastDiv (const Self &rhs)					C_NE___	{ return Self{ _mm256_mul_ps( _value, _mm256_rcp_ps( rhs._value ))}; }	// approx (a / b)

		// ordered - comparison with NaN returns false
		ND_ Bool8	Equal    (const Self &rhs)					C_NE___	{ return Bool8{ _mm256_cmp_ps( _value, rhs._value, _CMP_EQ_OQ  )}; }
		ND_ Bool8	NotEqual (const Self &rhs)					C_NE___	{ return Bool8{ _mm256_cmp_ps( _value, rhs._value, _CMP_NEQ_OQ )}; }
		ND_ Bool8	Greater  (const Self &rhs)					C_NE___	{ return Bool8{ _mm256_cmp_ps( _value, rhs._value, _CMP_GT_OQ  )}; }
		ND_ Bool8	Less     (const Self &rhs)					C_NE___	{ return Bool8{ _mm256_cmp_ps( _value, rhs._value, _CMP_LT_OQ  )}; }
		ND_ Bool8	GEqual   (const Self &rhs)					C_NE___	{ return Bool8{ _mm256_cmp_ps( _value, rhs._value, _CMP_GE_OQ  )}; }
		ND_ Bool8	LEqual   (const Self &rhs)					C_NE___	{ return Bool8{ _mm256_cmp_ps( _value, rhs._value, _CMP_LE_OQ  )}; }

		// unordered - comparison with NaN returns true
		ND_ Bool8	EqualU    (const Self &rhs)					C_NE___	{ return Bool8{ _mm256_cmp_ps( _value, rhs._value, _CMP_EQ_UQ  )}; }
		ND_ Bool8	NotEqualU (const Self &rhs)					C_NE___	{ return Bool8{ _mm256_cmp_ps( _value, rhs._value, _CMP_NEQ_UQ )}; }
		ND_ Bool8	GreaterU  (const Self &rhs)					C_NE___	{ return Bool8{ _mm256_cmp_ps( _value, rhs._value, _CMP_NLE_UQ )}; }
		ND_ Bool8	LessU     (const Self &rhs)					C_NE___	{ return Bool8{ _mm256_cmp_ps( _value, rhs._value, _CMP_NGE_UQ )}; }
		ND_ Bool8	GEqualU   (const Self &rhs)					C_NE___	{ return Bool8{ _mm256_cmp_ps( _value, rhs._value, _CMP_NLT_UQ )}; }
		ND_ Bool8	LEqualU   (const Self &rhs)					C_NE___	{ return Bool8{ _mm256_cmp_ps( _value, rhs._value, _CMP_NGT_UQ )}; }

		// compare and return 0.0 or 1.0
		ND_ Self	EqualF    (const Self &rhs)					C_NE___	{ return Self{ _mm256_and_ps( _mm256_cmp_ps( _value, rhs._value, _CMP_EQ_OQ  ), _mm256_set1_ps(1.f) )}; }
		ND_ Self	NotEqualF (const Self &rhs)					C_NE___	{ return Self{ _mm256_and_ps( _mm256_cmp_ps( _value, rhs._value, _CMP_NEQ_OQ ), _mm256_set1_ps(1.f) )}; }
		ND_ Self	GreaterF  (const Self &rhs)					C_NE___	{ return Self{ _mm256_and_ps( _mm256_cmp_ps( _value, rhs._value, _CMP_GT_OQ  ), _mm256_set1_ps(1.f) )}; }
		ND_ Self	LessF     (const Self &rhs)					C_NE___	{ return Self{ _mm256_and_ps( _mm256_cmp_ps( _value, rhs._value, _CMP_LT_OQ  ), _mm256_set1_ps(1.f) )}; }
		ND_ Self	GEqualF   (const Self &rhs)					C_NE___	{ return Self{ _mm256_and_ps( _mm256_cmp_ps( _value, rhs._value, _CMP_GE_OQ  ), _mm256_set1_ps(1.f) )}; }
		ND_ Self	LEqualF   (const Self &rhs)					C_NE___	{ return Self{ _mm256_and_ps( _mm256_cmp_ps( _value, rhs._value, _CMP_LE_OQ  ), _mm256_set1_ps(1.f) )}; }

		ND_ Bool8	BitEqual (const Self&, EnabledBitCount acc)	C_NE___;	// TODO

		ND_ Self	Floor ()									C_NE___	{ return Self{ _mm256_round_ps( _value, _MM_FROUND_FLOOR )}; }
		ND_ Self	Ceil ()										C_NE___	{ return Self{ _mm256_round_ps( _value, _MM_FROUND_CEIL  )}; }
		ND_ Self	Trunc ()									C_NE___	{ return Self{ _mm256_round_ps( _value, _MM_FROUND_TRUNC )}; }
		ND_ Self	Round ()									C_NE___	{ return Self{ _mm256_round_ps( _value, _MM_FROUND_NINT  )}; }
		ND_ Self	RoundEven ()								C_NE___	{ return Self{ _mm256_round_ps( _value, _MM_FROUND_RINT  )}; }

	  #if defined(AE_COMPILER_MSVC) and not defined(AE_COMPILER_CLANG_CL) // SVML library
		ND_ Self	Sin ()										C_NE___	{ return Self{ _mm256_sin_ps( _value )}; }
		ND_ Self	Cos ()										C_NE___	{ return Self{ _mm256_cos_ps( _value )}; }
		ND_ auto	SinCos ()									C_NE___	{ Native_t c, s = _mm256_sincos_ps( OUT &c, _value );  return Tuple{s,c}; }
		ND_ Self	Tan ()										C_NE___	{ return Self{ _mm256_tan_ps( _value )}; }
		ND_ Self	ASin ()										C_NE___	{ return Self{ _mm256_asin_ps( _value )}; }
		ND_ Self	ACos ()										C_NE___	{ return Self{ _mm256_acos_ps( _value )}; }
		ND_ Self	ATan ()										C_NE___	{ return Self{ _mm256_atan_ps( _value )}; }
		ND_ Self	ATan2 (const Self &x)						C_NE___	{ return Self{ _mm256_atan2_ps( _value, x._value )}; }

		ND_ Self	SinH ()										C_NE___	{ return Self{ _mm256_sinh_ps( _value )}; }
		ND_ Self	CosH ()										C_NE___	{ return Self{ _mm256_cosh_ps( _value )}; }
		ND_ Self	TanH ()										C_NE___	{ return Self{ _mm256_tanh_ps( _value )}; }
		ND_ Self	ASinH ()									C_NE___	{ return Self{ _mm256_asinh_ps( _value )}; }
		ND_ Self	ACosH ()									C_NE___	{ return Self{ _mm256_acosh_ps( _value )}; }
		ND_ Self	ATanH ()									C_NE___	{ return Self{ _mm256_atanh_ps( _value )}; }

		ND_ Self	Ln ()										C_NE___	{ return Self{ _mm256_log_ps( _value )}; }
		ND_ Self	Log2 ()										C_NE___	{ return Self{ _mm256_log2_ps( _value )}; }
		ND_ Self	Log10 ()									C_NE___	{ return Self{ _mm256_log10_ps( _value )}; }

		ND_ Self	Exp ()										C_NE___	{ return Self{ _mm256_exp_ps( _value )}; }
		ND_ Self	Exp2 ()										C_NE___	{ return Self{ _mm256_exp2_ps( _value )}; }
		ND_ Self	Exp10 ()									C_NE___	{ return Self{ _mm256_exp10_ps( _value )}; }

		ND_ Self	PreciseInvSqrt ()							C_NE___	{ return Self{ _mm256_invsqrt_ps( _value )}; }
		ND_ Self	Cbrt ()										C_NE___	{ return Self{ _mm256_cbrt_ps( _value )}; }
		ND_ Self	InvCbrt ()									C_NE___	{ return Self{ _mm256_invcbrt_ps( _value )}; }

		ND_ Self	Mod (const Self &rhs)						C_NE___	{ return Self{ _mm256_fmod_ps( _value, rhs._value )}; }
		ND_ Self	Pow (const Self &base)						C_NE___	{ return Self{ _mm256_pow_ps( _value, base._value )}; }
	  #endif

		// fused multiply (FM) / fused negative multiply (FNM)
		// requires 'FMA' cpu feature
	  #if AE_SIMD_FMA
		ND_ friend Self  FusedMulAdd    (const Self &a, const Self &b, const Self &c)	__NE___ { return Self{ _mm256_fmadd_ps(    a._value, b._value, c._value )}; }	// a * b + c
		ND_ friend Self  FusedMulSub    (const Self &a, const Self &b, const Self &c)	__NE___ { return Self{ _mm256_fmsub_ps(    a._value, b._value, c._value )}; }	// a * b - c
		ND_ friend Self  FusedMulAddSub (const Self &a, const Self &b, const Self &c)	__NE___ { return Self{ _mm256_fmaddsub_ps( a._value, b._value, c._value )}; }	// { a0 * b0 - c0, a1 * b1 + c1, a2 * b2 - c2, a3 * b3 + c3 }
		ND_ friend Self  FusedMulSubAdd (const Self &a, const Self &b, const Self &c)	__NE___ { return Self{ _mm256_fmaddsub_ps( a._value, b._value, c._value )}; }	// { a0 * b0 + c0, a1 * b1 - c1, a2 * b2 + c2, a3 * b3 - c3 }
		ND_ friend Self  FusedNegMulAdd (const Self &a, const Self &b, const Self &c)	__NE___ { return Self{ _mm256_fnmadd_ps(   a._value, b._value, c._value )}; }	// -a * b + c
		ND_ friend Self  FusedNegMulSub (const Self &a, const Self &b, const Self &c)	__NE___ { return Self{ _mm256_fnmsub_ps(   a._value, b._value, c._value )}; }	// -a * b - c
	  #endif

		ND_ friend Self  Lerp    (const Self &x, const Self &y, const Self &factor)						__NE___	{ return x * (Self{1.f} - factor) + y * factor; }
		ND_ friend Self  Select  (const Bool8 &condition, const Self &ifTrue, const Self &ifFalse)		__NE___	{ return Self{ _mm256_blendv_ps( ifFalse._value, ifTrue._value, condition.Ref() )}; }
		ND_ friend Self  SelectF (const Self &x, const Self &y, const Self &ifTrue, const Self &ifFalse)__NE___	{ return Lerp( ifFalse, ifTrue, x.LessF(y) ); }

		// sum/max of all lanes, result written to all lanes
		ND_ Self		ReduceAdd ()							C_NE___;	// GLSL: subgroup.Add
		ND_ Self		ReduceMax ()							C_NE___;	// GLSL: subgroup.Max
		ND_ Self		ReduceMin ()							C_NE___;	// GLSL: subgroup.Min

		// helper
		ND_ Scalar_t	ReduceAddScalar ()						C_NE___	{ return ReduceAdd().get<0>(); }
		ND_ Scalar_t	ReduceMaxScalar ()						C_NE___	{ return ReduceMax().get<0>(); }
		ND_ Scalar_t	ReduceMinScalar ()						C_NE___	{ return ReduceMin().get<0>(); }

		// set 'simd[lane] += simd[lane-1]'
		ND_ Self		InclusiveAdd ()							C_NE___;	// GLSL: subgroup.InclusiveAdd
		ND_ Self		ExclusiveAdd ()							C_NE___;	// GLSL: subgroup.ExclusiveAdd


	// per bit operations //
		ND_ Self	BitInverse ()								C_NE___	{ return AndNot( Bool8::True() ); }

		ND_ Self	And (const Self &rhs)						C_NE___	{ return Self{ _mm256_and_ps( _value, rhs._value )}; }
		ND_ Self	Or  (const Self &rhs)						C_NE___	{ return Self{ _mm256_or_ps( _value, rhs._value )}; }
		ND_ Self	Xor (const Self &rhs)						C_NE___	{ return Self{ _mm256_xor_ps( _value, rhs._value )}; }
		ND_ Self	AndNot (const Self &rhs)					C_NE___	{ return Self{ _mm256_andnot_ps( _value, rhs._value )}; }	// ~a & b

		ND_ Self	And (const Bool8 &rhs)						C_NE___	{ return Self{ _mm256_and_ps( _value, rhs.Ref() )}; }
		ND_ Self	Or  (const Bool8 &rhs)						C_NE___	{ return Self{ _mm256_or_ps( _value, rhs.Ref() )}; }
		ND_ Self	Xor (const Bool8 &rhs)						C_NE___	{ return Self{ _mm256_xor_ps( _value, rhs.Ref() )}; }
		ND_ Self	AndNot (const Bool8 &rhs)					C_NE___	{ return Self{ _mm256_andnot_ps( _value, rhs.Ref() )}; }	// ~a & b

		ND_ Self	And (const uint rhs)						C_NE___	{ return And( Self{Base::BitCast<Scalar_t>( rhs )} ); }
		ND_ Self	Or  (const uint rhs)						C_NE___	{ return Or(  Self{Base::BitCast<Scalar_t>( rhs )} ); }
		ND_ Self	Xor (const uint rhs)						C_NE___	{ return Xor( Self{Base::BitCast<Scalar_t>( rhs )} ); }
		ND_ Self	AndNot (const uint rhs)						C_NE___	{ return AndNot( Self{Base::BitCast<Scalar_t>( rhs )} ); }	// ~a & b


	// conversion //
		template <uint Idx>
		ND_ auto		Part ()									C_NE___ { StaticAssert( Idx < parts );  return SimdFloat4{ _mm256_extractf128_ps( _value, Idx )}; }

		template <uint Idx>
		ND_ auto		ToDouble ()								C_NE___	{ return Part<Idx>().ToDouble4(); }
	//	ND_ SimdDouble8	ToDouble8 ()							C_NE___;	// _mm512_cvtps_pd (AVX512F), _mm512_cvtepi64_pd (AVX512DQ)

		ND_ SimdInt8	ToInt ()								C_NE___;

	  #ifdef AE_SIMD_SimdHalf8
		ND_ SimdHalf8	ToHalf ()								C_NE___;
		ND_ explicit operator SimdHalf8 ()						C_NE___	{ return ToHalf(); }
		explicit SimdFloat8 (const SimdHalf8 &v)				__NE___	: _value{ v.ToFloat8()._value } {}
	  #endif

		ND_ Array_t	ToArray ()									C_NE___	{ Array_t arr;  _mm256_storeu_ps( OUT arr.data(), _value );  return arr; }
			void	ToArray (OUT Scalar_t* dst)					C_NE___	{ NonNull( dst );  _mm256_storeu_ps( OUT dst, _value ); }
			void	ToAlignedArray (OUT Ptr_t dst)				C_NE___	{ _mm256_store_ps( OUT dst.Cast<Scalar_t>(), _value ); }
			void	ToArray (OUT Scalar_t* dst, Mask_t mask)	C_NE___;

		template <uint V0, uint V1, uint V2, uint V3, uint V4, uint V5, uint V6, uint V7>
		ND_ Self	Swizzle ()									C_NE___;

		template <uint V0, uint V1, uint V2, uint V3, uint V4, uint V5, uint V6, uint V7>
		ND_ Self	Shuffle (const Self &v8)					C_NE___;

		template <uint Low, uint High>
		ND_ Self	SwizzleParts ()								C_NE___;

		template <typename DstType>
		ND_ DstType	BitCast ()									C_NE___;

		template <typename DstScalar>
		ND_ auto	Convert ()									C_NE___;


	// Features //
		NdCe__ static bool  Has_Arithmetic ()					{ return true; }
		NdCe__ static bool  Has_Equal ()						{ return true; }
		NdCe__ static bool  Has_Greater ()						{ return true; }
		NdCe__ static bool  Has_Rounding ()						{ return true; }
		NdCe__ static bool  Has_FusedMulAdd ()					{ return AE_SIMD_FMA > 0; }
		NdCe__ static bool  Has_MulAdd ()						{ return false; }
		NdCe__ static bool  Has_PreciseSqrt ()					{ return true; }
		NdCe__ static bool  Has_PreciseDiv ()					{ return true; }
		NdCe__ static bool  Has_ApproxReciprocal ()				{ return true; }
		NdCe__ static bool  Has_ApproxInvSqrt ()				{ return true; }
		NdCe__ static bool  Has_ReduceAdd ()					{ return true; }
		NdCe__ static bool  Has_ReduceMinMax ()					{ return true; }
		NdCe__ static bool  Has_InclusiveAdd ()					{ return true; }
		NdCe__ static bool  Has_BitEqual ()						{ return false; }
		NdCe__ static bool  Has_Swizzle ()						{ return AE_SIMD_AVX >= 2; }
		NdCe__ static bool  Has_Shuffle ()						{ return AE_SIMD_AVX >= 2; }

	  #if defined(AE_COMPILER_MSVC) and not defined(AE_COMPILER_CLANG_CL) // SVML library
		NdCe__ static bool  Has_PreciseInvSqrt ()				{ return true; }
		NdCe__ static bool  Has_Trigonometry ()					{ return true; }
		NdCe__ static bool  Has_Exponential ()					{ return true; }
	  #else
		NdCe__ static bool  Has_PreciseInvSqrt ()				{ return false; }
		NdCe__ static bool  Has_Trigonometry ()					{ return false; }
		NdCe__ static bool  Has_Exponential ()					{ return false; }
	  #endif

		template <typename DstType>	NdCe__ static bool  Has_BitCast ();
		template <typename DstType>	NdCe__ static bool  Has_Convert ();

	private:
		// different behavior for Float4 & Double4, keep private
		ND_ Self	HAdd (const Self &rhs)						C_NE___	{ return Self{ _mm256_hadd_ps( _value, rhs._value )}; }		// { a0 + a1, b0 + b1, a4 + a5, b4 + b5, ... }
		ND_ Self	HSub (const Self &rhs)						C_NE___	{ return Self{ _mm256_hsub_ps( _value, rhs._value )}; }		// { a0 - a1, b0 - b1, a4 - a5, b4 - b5, ... }
		ND_ Self	AddSub (const Self &rhs)					C_NE___	{ return Self{ _mm256_addsub_ps( _value, rhs._value )}; }	// { a0 - b0, a1 + b1, a2 - b2, a3 + b3, ... }
	};



	//
	// 256 bits double (AVX)
	//
	#define AE_SIMD_SimdDouble4
	struct SimdDouble4
	{
	// types
	public:
		static constexpr uint	count	= 4;
		static constexpr uint	parts	= 2;	// number of SSE vectors, cross lane operations may have additional cost

		using Scalar_t		= double;
		using Self			= SimdDouble4;
		using Native_t		= __m256d;
		using Array_t		= StaticArray< Scalar_t, count >;
		using Ptr_t			= AlignedPtr< sizeof(Native_t) >;
		using CPtr_t		= AlignedPtr< sizeof(Native_t), true >;
		using SimdInt_t		= SimdLong4;
		using SimdUInt_t	= SimdULong4;
		using Mask_t		= MSBMask< count, 0 >;
		StaticAssert( sizeof(Array_t) == sizeof(Native_t) );


		struct Bool4
		{
		private:
			Native_t	_value;		// ulong[4]

		public:
			explicit Bool4 (bool val)							__NE___	: _value{ val ? True()._value : False()._value } {}
			explicit Bool4 (const Native_t &val)				__NE___	: _value{ val } {}
			explicit Bool4 (const __m256i &val)					__NE___	: _value{ _mm256_castsi256_pd( val )} {}
			explicit Bool4 (const SimdInt_t &v)					__NE___;
			explicit Bool4 (const SimdUInt_t &v)				__NE___;
			Bool4 (bool v0, bool v1, bool v2, bool v3)			__NE___;

			ND_ Bool4	operator | (const Bool4 &rhs)			C_NE___	{ return Bool4{ _mm256_or_pd(  _value, rhs._value )}; }
			ND_ Bool4	operator & (const Bool4 &rhs)			C_NE___	{ return Bool4{ _mm256_and_pd( _value, rhs._value )}; }
			ND_ Bool4	operator ^ (const Bool4 &rhs)			C_NE___	{ return Bool4{ _mm256_xor_pd( _value, rhs._value )}; }
			ND_ Bool4	operator ! ()							C_NE___	{ return Bool4{ _mm256_andnot_pd( _value, True()._value )}; }

			ND_ Bool4	operator == (const Bool4 &rhs)			C_NE___	{ return Bool4{ _mm256_and_pd( _value, rhs._value )}; }
			ND_ Bool4	operator != (const Bool4 &rhs)			C_NE___	{ return Bool4{ _mm256_andnot_pd( _value, rhs._value )}; }

			ND_ Bool4	AndNot (const Bool4 &rhs)				C_NE___	{ return Bool4{ _mm256_andnot_pd( _value, rhs._value )}; }		// ~a & b
			ND_ Self	AndNot (const Self &rhs)				C_NE___	{ return Self{ _mm256_andnot_pd( _value, rhs.Ref() )}; }		// ~a & b

			ND_ bool	All ()									C_NE___	{ return _mm256_movemask_pd( _value ) == 0xF; }
			ND_ bool	Any ()									C_NE___	{ return _mm256_movemask_pd( _value ) != 0; }
			ND_ bool	None ()									C_NE___	{ return _mm256_movemask_pd( _value ) == 0; }

			ND_ auto&	Ref ()									C_NE___	{ return _value; }

			ND_ Mask_t	ToBitfield ()							C_NE___	{ return Mask_t{_mm256_movemask_pd( _value )}; }

			template <typename DstType>
			ND_ DstType	BitCast ()								C_NE___;

			ND_ static Bool4	True ()							__NE___	{ auto z = _mm256_setzero_pd();  return Bool4{_mm256_cmp_pd( z, z, _CMP_EQ_OQ )}; }
			ND_ static Bool4	False ()						__NE___	{ return Bool4{_mm256_setzero_pd()}; }
		};
		using Bool_t = Bool4;


	// variables
	private:
		Native_t	_value;		// double[4]


	// methods
	public:
		SimdDouble4 ()											__NE___	: _value{ _mm256_setzero_pd() } {}
		SimdDouble4 (Zero_t)									__NE___	: _value{ _mm256_setzero_pd() } {}
		explicit SimdDouble4 (Scalar_t v)						__NE___	: _value{ _mm256_set1_pd( v )} {}
		explicit SimdDouble4 (const Native_t &v)				__NE___	: _value{ v } {}
		explicit SimdDouble4 (const Scalar_t* ptr)				__NE___	: _value{ _mm256_loadu_pd( GetNonNull( ptr ))} {}
		explicit SimdDouble4 (const CPtr_t ptr)					__NE___	: _value{ _mm256_load_pd( ptr.Cast<Scalar_t>() )} {}
		SimdDouble4 (Scalar_t x, Scalar_t y, Scalar_t z, Scalar_t w) __NE___ : _value{ _mm256_set_pd( w, z, y, x )} {}

		explicit SimdDouble4 (const SimdDouble2 &low)			__NE___;
		SimdDouble4 (const SimdDouble2 &low, const SimdDouble2 &high) __NE___;

		ND_ Self	operator +  (const Self &rhs)				C_NE___	{ return Add( rhs ); }
		ND_ Self	operator -  (const Self &rhs)				C_NE___	{ return Sub( rhs ); }
		ND_ Self	operator *  (const Self &rhs)				C_NE___	{ return Mul( rhs ); }
		ND_ Self	operator /  (const Self &rhs)				C_NE___	{ return PreciseDiv( rhs ); }

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

		template <uint I> ND_ Scalar_t	get ()					C_NE___;
		template <uint I> ND_ Self		set (Scalar_t val)		C_NE___;

		ND_ Native_t &		Ref ()								__NE___	{ return _value; }
		ND_ Native_t const&	Ref ()								C_NE___	{ return _value; }

		ND_ Self	Add (const Self &rhs)						C_NE___	{ return Self{ _mm256_add_pd( _value, rhs._value )}; }
		ND_ Self	Sub (const Self &rhs)						C_NE___	{ return Self{ _mm256_sub_pd( _value, rhs._value )}; }
		ND_ Self	Mul (const Self &rhs)						C_NE___	{ return Self{ _mm256_mul_pd( _value, rhs._value )}; }
		ND_ Self	PreciseDiv (const Self &rhs)				C_NE___	{ return Self{ _mm256_div_pd( _value, rhs._value )}; }
		ND_ Self	Min (const Self &rhs)						C_NE___	{ return Self{ _mm256_min_pd( _value, rhs._value )}; }
		ND_ Self	Max (const Self &rhs)						C_NE___	{ return Self{ _mm256_max_pd( _value, rhs._value )}; }

		ND_ Self	Add (Scalar_t rhs)							C_NE___	{ return Self{ _mm256_add_pd( _value, _mm256_set1_pd(rhs) )}; }
		ND_ Self	Sub (Scalar_t rhs)							C_NE___	{ return Self{ _mm256_sub_pd( _value, _mm256_set1_pd(rhs) )}; }
		ND_ Self	Mul (Scalar_t rhs)							C_NE___	{ return Self{ _mm256_mul_pd( _value, _mm256_set1_pd(rhs) )}; }
		ND_ Self	PreciseDiv (Scalar_t rhs)					C_NE___	{ return Self{ _mm256_div_pd( _value, _mm256_set1_pd(rhs) )}; }
		ND_ Self	Min (Scalar_t rhs)							C_NE___	{ return Self{ _mm256_min_pd( _value, _mm256_set1_pd(rhs) )}; }
		ND_ Self	Max (Scalar_t rhs)							C_NE___	{ return Self{ _mm256_max_pd( _value, _mm256_set1_pd(rhs) )}; }

		ND_ Self	Abs ()										C_NE___	{ return Self{ _mm256_andnot_pd( _mm256_set1_pd( -0.0 ), _value )}; }
		ND_ Self	PreciseSqrt ()								C_NE___	{ return Self{ _mm256_sqrt_pd( _value )}; }

		ND_ Self	Reciprocal_fp32 ()							C_NE___;
	  #if AE_SIMD_AVX >= 31	// AVX512VL
		ND_ Self	Reciprocal ()								C_NE___	{ return Self{ _mm256_rcp14_pd( _value )}; }	// approx (1 / x)
	  #else
		ND_ Self	Reciprocal ()								C_NE___	{ return Reciprocal_fp32(); }
	  #endif
		ND_ Self	FastDiv (const Self &rhs)					C_NE___	{ return *this * rhs.Reciprocal(); }			// approx (a / b)

		// ordered - comparison with NaN returns false
		ND_ Bool4	Equal    (const Self &rhs)					C_NE___	{ return Bool4{ _mm256_cmp_pd( _value, rhs._value, _CMP_EQ_OQ  )}; }
		ND_ Bool4	NotEqual (const Self &rhs)					C_NE___	{ return Bool4{ _mm256_cmp_pd( _value, rhs._value, _CMP_NEQ_OQ )}; }
		ND_ Bool4	Greater  (const Self &rhs)					C_NE___	{ return Bool4{ _mm256_cmp_pd( _value, rhs._value, _CMP_GT_OQ  )}; }
		ND_ Bool4	Less     (const Self &rhs)					C_NE___	{ return Bool4{ _mm256_cmp_pd( _value, rhs._value, _CMP_LT_OQ  )}; }
		ND_ Bool4	GEqual   (const Self &rhs)					C_NE___	{ return Bool4{ _mm256_cmp_pd( _value, rhs._value, _CMP_GE_OQ  )}; }
		ND_ Bool4	LEqual   (const Self &rhs)					C_NE___	{ return Bool4{ _mm256_cmp_pd( _value, rhs._value, _CMP_LE_OQ  )}; }

		// unordered - comparison with NaN returns true
		ND_ Bool4	EqualU    (const Self &rhs)					C_NE___	{ return Bool4{ _mm256_cmp_pd( _value, rhs._value, _CMP_EQ_UQ  )}; }
		ND_ Bool4	NotEqualU (const Self &rhs)					C_NE___	{ return Bool4{ _mm256_cmp_pd( _value, rhs._value, _CMP_NEQ_UQ )}; }
		ND_ Bool4	GreaterU  (const Self &rhs)					C_NE___	{ return Bool4{ _mm256_cmp_pd( _value, rhs._value, _CMP_NLE_UQ )}; }
		ND_ Bool4	LessU     (const Self &rhs)					C_NE___	{ return Bool4{ _mm256_cmp_pd( _value, rhs._value, _CMP_NGE_UQ )}; }
		ND_ Bool4	GEqualU   (const Self &rhs)					C_NE___	{ return Bool4{ _mm256_cmp_pd( _value, rhs._value, _CMP_NLT_UQ )}; }
		ND_ Bool4	LEqualU   (const Self &rhs)					C_NE___	{ return Bool4{ _mm256_cmp_pd( _value, rhs._value, _CMP_NGT_UQ )}; }

		// compare and return 0.0 or 1.0
		ND_ Self	EqualF    (const Self &rhs)					C_NE___	{ return Self{ _mm256_and_pd( _mm256_cmp_pd( _value, rhs._value, _CMP_EQ_UQ  ), _mm256_set1_pd(1.0) )}; }
		ND_ Self	NotEqualF (const Self &rhs)					C_NE___	{ return Self{ _mm256_and_pd( _mm256_cmp_pd( _value, rhs._value, _CMP_NEQ_UQ ), _mm256_set1_pd(1.0) )}; }
		ND_ Self	GreaterF  (const Self &rhs)					C_NE___	{ return Self{ _mm256_and_pd( _mm256_cmp_pd( _value, rhs._value, _CMP_NLE_UQ ), _mm256_set1_pd(1.0) )}; }
		ND_ Self	LessF     (const Self &rhs)					C_NE___	{ return Self{ _mm256_and_pd( _mm256_cmp_pd( _value, rhs._value, _CMP_NGE_UQ ), _mm256_set1_pd(1.0) )}; }
		ND_ Self	GEqualF   (const Self &rhs)					C_NE___	{ return Self{ _mm256_and_pd( _mm256_cmp_pd( _value, rhs._value, _CMP_NLT_UQ ), _mm256_set1_pd(1.0) )}; }
		ND_ Self	LEqualF   (const Self &rhs)					C_NE___	{ return Self{ _mm256_and_pd( _mm256_cmp_pd( _value, rhs._value, _CMP_NGT_UQ ), _mm256_set1_pd(1.0) )}; }

		ND_ Bool4	BitEqual (const Self&, EnabledBitCount acc)	C_NE___;	// TODO

	  #if defined(AE_COMPILER_MSVC) and not defined(AE_COMPILER_CLANG_CL) // SVML library
		ND_ Self	Sin ()										C_NE___	{ return Self{ _mm256_sin_pd( _value )}; }
		ND_ Self	Cos ()										C_NE___	{ return Self{ _mm256_cos_pd( _value )}; }
		ND_ auto	SinCos ()									C_NE___	{ Native_t c, s = _mm256_sincos_pd( OUT &c, _value );  return Tuple{s,c}; }
		ND_ Self	Tan ()										C_NE___	{ return Self{ _mm256_tan_pd( _value )}; }
		ND_ Self	ASin ()										C_NE___	{ return Self{ _mm256_asin_pd( _value )}; }
		ND_ Self	ACos ()										C_NE___	{ return Self{ _mm256_acos_pd( _value )}; }
		ND_ Self	ATan ()										C_NE___	{ return Self{ _mm256_atan_pd( _value )}; }
		ND_ Self	ATan2 (const Self &x)						C_NE___	{ return Self{ _mm256_atan2_pd( _value, x._value )}; }

		ND_ Self	SinH ()										C_NE___	{ return Self{ _mm256_sinh_pd( _value )}; }
		ND_ Self	CosH ()										C_NE___	{ return Self{ _mm256_cosh_pd( _value )}; }
		ND_ Self	TanH ()										C_NE___	{ return Self{ _mm256_tanh_pd( _value )}; }
		ND_ Self	ASinH ()									C_NE___	{ return Self{ _mm256_asinh_pd( _value )}; }
		ND_ Self	ACosH ()									C_NE___	{ return Self{ _mm256_acosh_pd( _value )}; }
		ND_ Self	ATanH ()									C_NE___	{ return Self{ _mm256_atanh_pd( _value )}; }

		ND_ Self	Ln ()										C_NE___	{ return Self{ _mm256_log_pd( _value )}; }
		ND_ Self	Log2 ()										C_NE___	{ return Self{ _mm256_log2_pd( _value )}; }
		ND_ Self	Log10 ()									C_NE___	{ return Self{ _mm256_log10_pd( _value )}; }

		ND_ Self	Exp ()										C_NE___	{ return Self{ _mm256_exp_pd( _value )}; }
		ND_ Self	Exp2 ()										C_NE___	{ return Self{ _mm256_exp2_pd( _value )}; }
		ND_ Self	Exp10 ()									C_NE___	{ return Self{ _mm256_exp10_pd( _value )}; }

		ND_ Self	PreciseInvSqrt ()							C_NE___	{ return Self{ _mm256_invsqrt_pd( _value )}; }
		ND_ Self	Cbrt ()										C_NE___	{ return Self{ _mm256_cbrt_pd( _value )}; }
		ND_ Self	InvCbrt ()									C_NE___	{ return Self{ _mm256_invcbrt_pd( _value )}; }

		ND_ Self	Mod (const Self &rhs)						C_NE___	{ return Self{ _mm256_fmod_pd( _value, rhs._value )}; }
		ND_ Self	Pow (const Self &base)						C_NE___	{ return Self{ _mm256_pow_pd( _value, base._value )}; }
	  #endif

		// fused multiply (FM) / fused negative multiply (FNM)
		// requires 'FMA' cpu feature
	  #if AE_SIMD_FMA
		ND_ friend Self  FusedMulAdd    (const Self &a, const Self &b, const Self &c)	__NE___ { return Self{ _mm256_fmadd_pd(    a._value, b._value, c._value )}; }	// a * b + c
		ND_ friend Self  FusedMulSub    (const Self &a, const Self &b, const Self &c)	__NE___ { return Self{ _mm256_fmsub_pd(    a._value, b._value, c._value )}; }	// a * b - c
		ND_ friend Self  FusedMulAddSub (const Self &a, const Self &b, const Self &c)	__NE___ { return Self{ _mm256_fmaddsub_pd( a._value, b._value, c._value )}; }	// { a0 * b0 - c0, a1 * b1 + c1, a2 * b2 - c2, a3 * b3 + c3 }
		ND_ friend Self  FusedMulSubAdd (const Self &a, const Self &b, const Self &c)	__NE___ { return Self{ _mm256_fmaddsub_pd( a._value, b._value, c._value )}; }	// { a0 * b0 + c0, a1 * b1 - c1, a2 * b2 + c2, a3 * b3 - c3 }
		ND_ friend Self  FusedNegMulAdd (const Self &a, const Self &b, const Self &c)	__NE___ { return Self{ _mm256_fnmadd_pd(   a._value, b._value, c._value )}; }	// -a * b + c
		ND_ friend Self  FusedNegMulSub (const Self &a, const Self &b, const Self &c)	__NE___ { return Self{ _mm256_fnmsub_pd(   a._value, b._value, c._value )}; }	// -a * b - c
	  #endif

		ND_ friend Self  Lerp    (const Self &x, const Self &y, const Self &factor)						__NE___	{ return x * (Self{1.0} - factor) + y * factor; }
		ND_ friend Self  Select  (const Bool4 &condition, const Self &ifTrue, const Self &ifFalse)		__NE___	{ return Self{ _mm256_blendv_pd( ifFalse._value, ifTrue._value, condition.Ref() )}; }
		ND_ friend Self  SelectF (const Self &x, const Self &y, const Self &ifTrue, const Self &ifFalse)__NE___	{ return Lerp( ifFalse, ifTrue, x.LessF(y) ); }


		// sum/max of all lanes, result written to all lanes
		ND_ Self		ReduceAdd ()							C_NE___;	// GLSL: subgroup.Add
		ND_ Self		ReduceMax ()							C_NE___;	// GLSL: subgroup.Max
		ND_ Self		ReduceMin ()							C_NE___;	// GLSL: subgroup.Min

		// helper
		ND_ Scalar_t	ReduceAddScalar ()						C_NE___	{ return ReduceAdd().get<0>(); }
		ND_ Scalar_t	ReduceMaxScalar ()						C_NE___	{ return ReduceMax().get<0>(); }
		ND_ Scalar_t	ReduceMinScalar ()						C_NE___	{ return ReduceMin().get<0>(); }

		// set 'simd[lane] += simd[lane-1]'
		ND_ Self		InclusiveAdd ()							C_NE___;	// GLSL: subgroup.InclusiveAdd
		ND_ Self		ExclusiveAdd ()							C_NE___;	// GLSL: subgroup.ExclusiveAdd


		ND_ Self	Floor ()									C_NE___	{ return Self{ _mm256_round_pd( _value, _MM_FROUND_FLOOR )}; }
		ND_ Self	Ceil ()										C_NE___	{ return Self{ _mm256_round_pd( _value, _MM_FROUND_CEIL  )}; }
		ND_ Self	Trunc ()									C_NE___	{ return Self{ _mm256_round_pd( _value, _MM_FROUND_TRUNC )}; }
		ND_ Self	Round ()									C_NE___	{ return Self{ _mm256_round_pd( _value, _MM_FROUND_NINT  )}; }
		ND_ Self	RoundEven ()								C_NE___	{ return Self{ _mm256_round_pd( _value, _MM_FROUND_RINT  )}; }


	// per bit operations //
		ND_ Self	BitInverse ()								C_NE___	{ return AndNot( Bool4::True() ); }

		ND_ Self	And (const Self &rhs)						C_NE___	{ return Self{ _mm256_and_pd( _value, rhs._value )}; }
		ND_ Self	Or  (const Self &rhs)						C_NE___	{ return Self{ _mm256_or_pd( _value, rhs._value )}; }
		ND_ Self	Xor (const Self &rhs)						C_NE___	{ return Self{ _mm256_xor_pd( _value, rhs._value )}; }
		ND_ Self	AndNot (const Self &rhs)					C_NE___	{ return Self{ _mm256_andnot_pd( _value, rhs._value )}; }	// ~a & b

		ND_ Self	And (const Bool4 &rhs)						C_NE___	{ return Self{ _mm256_and_pd( _value, rhs.Ref() )}; }
		ND_ Self	Or  (const Bool4 &rhs)						C_NE___	{ return Self{ _mm256_or_pd( _value, rhs.Ref() )}; }
		ND_ Self	Xor (const Bool4 &rhs)						C_NE___	{ return Self{ _mm256_xor_pd( _value, rhs.Ref() )}; }
		ND_ Self	AndNot (const Bool4 &rhs)					C_NE___	{ return Self{ _mm256_andnot_pd( _value, rhs.Ref() )}; }	// ~a & b

		ND_ Self	And (const ulong rhs)						C_NE___	{ return And( Self{Base::BitCast<Scalar_t>( rhs )} ); }
		ND_ Self	Or  (const ulong rhs)						C_NE___	{ return Or(  Self{Base::BitCast<Scalar_t>( rhs )} ); }
		ND_ Self	Xor (const ulong rhs)						C_NE___	{ return Xor( Self{Base::BitCast<Scalar_t>( rhs )} ); }
		ND_ Self	AndNot (const ulong rhs)					C_NE___	{ return AndNot( Self{Base::BitCast<Scalar_t>( rhs )} ); }	// ~a & b


	// conversion //
		template <uint X, uint Y, uint Z, uint W>
		ND_ Self	Swizzle ()									C_NE___;

		template <uint Low, uint High>
		ND_ Self	SwizzleParts ()								C_NE___;

		template <uint X, uint Y, uint Z, uint W>
		ND_ Self	Shuffle (const Self &b)						C_NE___;

		template <uint Idx>
		ND_ auto	Part ()										C_NE___ { StaticAssert( Idx < parts );  return SimdDouble2{ _mm256_extractf128_pd( _value, Idx )}; }

		ND_ Array_t	ToArray ()									C_NE___	{ Array_t arr;  _mm256_storeu_pd( OUT arr.data(), _value );  return arr; }
			void	ToArray (OUT Scalar_t* dst)					C_NE___	{ NonNull( dst );  _mm256_storeu_pd( OUT dst, _value ); }
			void	ToAlignedArray (OUT Scalar_t* dst)			C_NE___	{ CheckPointerCast<Native_t>( dst );  _mm256_store_pd( OUT dst, _value ); }

		ND_ explicit operator packed_double4 ()					C_NE___	{ packed_double4 tmp;  _mm256_storeu_pd( OUT &tmp.x, _value );  return tmp; }

		ND_ SimdFloat4	ToFloat ()								C_NE___;
		ND_ SimdLong4	ToLong ()								C_NE___;	// AVX512 only, check 'Has_Convert<SimdLong4>()'
		ND_ SimdInt4	ToInt ()								C_NE___;

		template <typename DstType>
		ND_ DstType	BitCast ()									C_NE___;

		template <typename DstScalar>
		ND_ auto	Convert ()									C_NE___;


	// Features //
		NdCe__ static bool  Has_Arithmetic ()					{ return true; }
		NdCe__ static bool  Has_Equal ()						{ return true; }
		NdCe__ static bool  Has_Greater ()						{ return true; }
		NdCe__ static bool  Has_Rounding ()						{ return true; }
		NdCe__ static bool  Has_FusedMulAdd ()					{ return AE_SIMD_FMA > 0; }
		NdCe__ static bool  Has_MulAdd ()						{ return false; }
		NdCe__ static bool  Has_PreciseSqrt ()					{ return true; }
		NdCe__ static bool  Has_PreciseDiv ()					{ return true; }
		NdCe__ static bool  Has_ApproxReciprocal ()				{ return AE_SIMD_AVX >= 31; }
		NdCe__ static bool  Has_ApproxInvSqrt ()				{ return false; }
		NdCe__ static bool  Has_ReduceAdd ()					{ return Has_Swizzle(); }
		NdCe__ static bool  Has_ReduceMinMax ()					{ return Has_Swizzle(); }
		NdCe__ static bool  Has_InclusiveAdd ()					{ return true; }
		NdCe__ static bool  Has_BitEqual ()						{ return false; }
		NdCe__ static bool  Has_Swizzle ()						{ return true; }
		NdCe__ static bool  Has_Shuffle ()						{ return true; }

	  #if defined(AE_COMPILER_MSVC) and not defined(AE_COMPILER_CLANG_CL) // SVML library
		NdCe__ static bool  Has_PreciseInvSqrt ()				{ return true; }
		NdCe__ static bool  Has_Trigonometry ()					{ return true; }
		NdCe__ static bool  Has_Exponential ()					{ return true; }
	  #else
		NdCe__ static bool  Has_PreciseInvSqrt ()				{ return false; }
		NdCe__ static bool  Has_Trigonometry ()					{ return false; }
		NdCe__ static bool  Has_Exponential ()					{ return false; }
	  #endif

		template <typename DstType>	NdCe__ static bool  Has_BitCast ();
		template <typename DstType>	NdCe__ static bool  Has_Convert ();

	private:
		// different behavior for Float4 & Double4, keep private
		ND_ Self	HAdd (const Self &rhs)						C_NE___	{ return Self{ _mm256_hadd_pd( _value, rhs._value )}; }		// { a0 + a1, b0 + b1, a2 + a3, b2 + b3 }
		ND_ Self	HSub (const Self &rhs)						C_NE___	{ return Self{ _mm256_hsub_pd( _value, rhs._value )}; }		// { a0 - a1, b0 - b1, a2 - a3, b2 - b3 }
		ND_ Self	AddSub (const Self &rhs)					C_NE___	{ return Self{ _mm256_addsub_pd( _value, rhs._value )}; }	// { a0 - b0, a1 + b1, a2 - b2, a3 + b3 }

	};
//-----------------------------------------------------------------------------



	//
	// 256 bit int
	//
	#define AE_SIMD_Int256b
	struct Int256b
	{
	// types
	public:
		using Self		= Int256b;
		using Native_t	= __m256i;
		using Ptr_t		= AlignedPtr< sizeof(Native_t) >;
		using CPtr_t	= AlignedPtr< sizeof(Native_t), true >;


	// variables
	private:
		Native_t	_value;


	// methods
	public:
		Int256b ()											__NE___	: _value{ _mm256_setzero_si256() } {}
		Int256b (Zero_t)									__NE___	: _value{ _mm256_setzero_si256() } {}
		Int256b (UMax_t)									__NE___	 { auto z = _mm256_setzero_si256();  _value = _mm256_cmpeq_epi32( z, z ); }
		explicit Int256b (const Native_t &v)				__NE___	: _value{ v } {}
		explicit Int256b (const CPtr_t ptr)					__NE___	: _value{ _mm256_load_si256( ptr.Cast<Native_t>() )} {}

		ND_ Native_t&		Ref ()							__NE___	{ return _value; }
		ND_ Native_t const&	Ref ()							C_NE___	{ return _value; }

		template <uint I> ND_ bool	GetBit ()				C_NE___;
		template <uint I> ND_ Self	SetBit (bool val)		C_NE___;


	// per bit operations //
		ND_ Self	operator ~ ()							C_NE___	{ return BitInverse(); }

		ND_ Self	operator & (const Self &rhs)			C_NE___	{ return And( rhs ); }
		ND_ Self	operator | (const Self &rhs)			C_NE___	{ return Or( rhs ); }
		ND_ Self	operator ^ (const Self &rhs)			C_NE___	{ return Xor( rhs ); }

		ND_ Self	BitInverse ()							C_NE___	{ return AndNot( Self{UMax} ); }

		ND_ Self	And (const Self &rhs)					C_NE___	{ return Self{ _mm256_and_si256( _value, rhs._value )}; }
		ND_ Self	Or  (const Self &rhs)					C_NE___	{ return Self{ _mm256_or_si256( _value, rhs._value )}; }
		ND_ Self	Xor (const Self &rhs)					C_NE___	{ return Self{ _mm256_xor_si256( _value, rhs._value )}; }
		ND_ Self	AndNot (const Self &rhs)				C_NE___	{ return Self{ _mm256_andnot_si256( _value, rhs._value )}; } // ~a & b

		// return 0 or 1
		ND_ int		TestAnd (const Self &mask)				C_NE___	{ return _mm256_testz_si256( _value, mask._value ); }		// a & mask == 0 ? 1 : 0
		ND_ int		TestAndNot (const Self &mask)			C_NE___	{ return _mm256_testc_si256( _value, mask._value ); }		// a & ~mask == 0 ? 1 : 0
		ND_ int		TestBoth_And_AndNot (const Self &mask)	C_NE___	{ return _mm256_testnzc_si256( _value, mask._value ); }		// (a & mask == 0) and (a & ~mask == 0) ? 1 : 0

		// shift each 128bit lane in bytes
		template <uint ShiftBytes> ND_ Self  LByteShift ()	C_NE___;
		template <uint ShiftBytes> ND_ Self  RByteShift ()	C_NE___;


	// conversion //
		template <typename T>
		ND_ auto	ToArray ()								C_NE___;
	};



	//
	// 256 bit integer (AVX 2)
	//
	#define AE_SIMD_SimdTInt256
	template <typename IntType>
	struct SimdTInt256
	{
		StaticAssert(( IsSame<IntType, Int128b> or IsInteger<IntType> ));

	// types
	public:
		using Scalar_t		= IntType;
		using Self			= SimdTInt256< IntType >;
		using Bool_t		= SimdTInt256< ToUnsignedInteger< IntType >>;
		using Native_t		= __m256i;
		using Ptr_t			= AlignedPtr< sizeof(Native_t) >;
		using CPtr_t		= AlignedPtr< sizeof(Native_t), true >;
		using Signed_t		= SimdTInt256< ToSignedInteger< IntType >>;
		using Unsigned_t	= SimdTInt256< ToUnsignedInteger< IntType >>;
		using Shift64_t		= SimdTInt128< slong >;

		static constexpr uint	parts	= 2;	// number of SSE vectors, cross lane operations may have additional cost
		static constexpr uint	count	= sizeof(Native_t) / sizeof(IntType);
		using Array_t					= StaticArray< Scalar_t, count >;
		StaticAssert( sizeof(Array_t) == sizeof(Native_t) );

		using Mask_t		= MSBMask< count, CT_IntLog2< 32/count >>;


	// variables
	private:
		Native_t	_value;		// int128[2], int64[4], int32[8], int16[16], int8[32]

		static constexpr bool	is8   = sizeof( IntType ) == 1;
		static constexpr bool	is16  = sizeof( IntType ) == 2;
		static constexpr bool	is32  = sizeof( IntType ) == 4;
		static constexpr bool	is64  = sizeof( IntType ) == 8;
		static constexpr bool	is128 = sizeof( IntType ) == 16;

		static constexpr bool	isU8  = IsSame< IntType, ubyte >;
		static constexpr bool	isU16 = IsSame< IntType, ushort >;
		static constexpr bool	isU32 = IsSame< IntType, uint >;
		static constexpr bool	isU64 = IsSame< IntType, ulong >;

		static constexpr bool	isI8  = IsSame< IntType, sbyte >;
		static constexpr bool	isI16 = IsSame< IntType, sshort >;
		static constexpr bool	isI32 = IsSame< IntType, sint >;
		static constexpr bool	isI64 = IsSame< IntType, slong >;

		StaticAssert( is8 or is16 or is32 or is64 or is128 );
		StaticAssert( isU8 or isU16 or isU32 or isU64 or isI8 or isI16 or isI32 or isI64 );


	// methods
	public:
		SimdTInt256 ()										__NE___	: _value{ _mm256_setzero_si256() } {}
		SimdTInt256 (Zero_t)								__NE___	: _value{ _mm256_setzero_si256() } {}
		SimdTInt256 (UMax_t)								__NE___	 { auto z = _mm256_setzero_si256();  _value = _mm256_cmpeq_epi32( z, z ); }
		explicit SimdTInt256 (const Native_t &v)			__NE___	: _value{ v } {}
		explicit SimdTInt256 (const CPtr_t ptr)				__NE___	: _value{ _mm256_load_si256( ptr.Cast<Native_t>() )} {}
		explicit SimdTInt256 (const Scalar_t* ptr)			__NE___ : _value{ _mm256_loadu_si256( reinterpret_cast<Native_t const *>( GetNonNull( ptr )) )} {}
		explicit SimdTInt256 (Scalar_t v)					__NE___;

		template <uint Step>
		explicit SimdTInt256 (MSBMask<count, Step> mask)	__NE___;

		explicit SimdTInt256 (const SimdTInt128<Scalar_t> &low) __NE___;
		SimdTInt256 (const SimdTInt128<Scalar_t> &low, const SimdTInt128<Scalar_t> &high) __NE___;

		template <typename T = Scalar_t> requires( sizeof(T)==2 )
		explicit SimdTInt256 (const SimdDouble4::Bool4 &v)	__NE___ : _value{ _mm256_castpd_si256( v.Ref() )} {}

		template <typename T = Scalar_t> requires( sizeof(T)==4 )
		explicit SimdTInt256 (const SimdFloat8::Bool8 &v)	__NE___ : _value{ _mm256_castps_si256( v.Ref() )} {}

		template <typename T = Scalar_t> requires( sizeof(T)==1 )
		SimdTInt256 (Scalar_t v00, Scalar_t v01, Scalar_t v02, Scalar_t v03,
					 Scalar_t v04, Scalar_t v05, Scalar_t v06, Scalar_t v07,
					 Scalar_t v08, Scalar_t v09, Scalar_t v10, Scalar_t v11,
					 Scalar_t v12, Scalar_t v13, Scalar_t v14, Scalar_t v15,
					 Scalar_t v16, Scalar_t v17, Scalar_t v18, Scalar_t v19,
					 Scalar_t v20, Scalar_t v21, Scalar_t v22, Scalar_t v23,
					 Scalar_t v24, Scalar_t v25, Scalar_t v26, Scalar_t v27,
					 Scalar_t v28, Scalar_t v29, Scalar_t v30, Scalar_t v31) __NE___ :
			_value{ _mm256_set_epi8( v31, v30, v29, v28, v27, v26, v25, v24,
									 v23, v22, v21, v20, v19, v18, v17, v16,
									 v15, v14, v13, v12, v11, v10, v09, v08,
									 v07, v06, v05, v04, v03, v02, v01, v00)} {}

		template <typename T = Scalar_t> requires( sizeof(T)==2 )
		SimdTInt256 (Scalar_t v00, Scalar_t v01, Scalar_t v02, Scalar_t v03,
					 Scalar_t v04, Scalar_t v05, Scalar_t v06, Scalar_t v07,
					 Scalar_t v08, Scalar_t v09, Scalar_t v10, Scalar_t v11,
					 Scalar_t v12, Scalar_t v13, Scalar_t v14, Scalar_t v15) __NE___ :
			_value{ _mm256_set_epi16( v15, v14, v13, v12, v11, v10, v09, v08,
									  v07, v06, v05, v04, v03, v02, v01, v00 )} {}

		template <typename T = Scalar_t> requires( sizeof(T)==4 )
		SimdTInt256 (Scalar_t v0, Scalar_t v1, Scalar_t v2, Scalar_t v3,
					 Scalar_t v4, Scalar_t v5, Scalar_t v6, Scalar_t v7) __NE___ :
			_value{ _mm256_set_epi32( v7, v6, v5, v4, v3, v2, v1, v0 )} {}

		template <typename T = Scalar_t> requires( sizeof(T)==8 )
		SimdTInt256 (Scalar_t v0, Scalar_t v1, Scalar_t v2, Scalar_t v3) __NE___ :
			_value{ _mm256_set_epi64x( v3, v2, v1, v0 )} {}

		template <typename T = Scalar_t> requires( sizeof(T)==16 )
		SimdTInt256 (Scalar_t v0, Scalar_t v1)				__NE___	:
			_value{ _mm256_set_m128( v1, v0 )} {}


		template <typename T = Scalar_t> requires( IsUnsignedInteger<T> )
		explicit SimdTInt256 (bool v)						__NE___ : SimdTInt256{ v ? Scalar_t{UMax} : Scalar_t{0} } {}

		template <typename B, typename T=Scalar_t> requires( IsSame< T, ubyte > and IsSame< B, bool >)
		SimdTInt256 (B v00, B v01, B v02, B v03, B v04, B v05, B v06, B v07,
					 B v08, B v09, B v10, B v11, B v12, B v13, B v14, B v15,
					 B v16, B v17, B v18, B v19, B v20, B v21, B v22, B v23,
					 B v24, B v25, B v26, B v27, B v28, B v29, B v30, B v31) __NE___;

		template <typename B, typename T=Scalar_t> requires( IsSame< T, ushort > and IsSame< B, bool >)
		SimdTInt256 (B v00, B v01, B v02, B v03, B v04, B v05, B v06, B v07,
					 B v08, B v09, B v10, B v11, B v12, B v13, B v14, B v15) __NE___;

		template <typename B, typename T=Scalar_t> requires( IsSame< T, uint > and IsSame< B, bool >)
		SimdTInt256 (B v0, B v1, B v2, B v3, B v4, B v5, B v6, B v7)		__NE___;

		template <typename B, typename T=Scalar_t> requires( IsSame< T, ulong > and IsSame< B, bool >)
		SimdTInt256 (B v0, B v1, B v2, B v3)				__NE___;


	  #if AE_SIMD_AVX >= 2
		template <typename T = Scalar_t> requires( IsSignedInteger<T> )
		ND_ Self	operator - ()							C_NE___	{ return Negative(); }

		template <typename T = Scalar_t> requires( IsUnsignedInteger<T> )
		ND_ Bool_t	operator ! ()							C_NE___	{ return BitInverse(); }

		ND_ Self	operator ~ ()							C_NE___	{ return BitInverse(); }

		ND_ Self	operator + (const Self &rhs)			C_NE___	{ return Add( rhs ); }
		ND_ Self	operator - (const Self &rhs)			C_NE___	{ return Sub( rhs ); }
		ND_ Self	operator * (const Self &rhs)			C_NE___	{ return Mul( rhs ); }

		ND_ Self	operator + (Scalar_t rhs)				C_NE___	{ return Add( rhs ); }
		ND_ Self	operator - (Scalar_t rhs)				C_NE___	{ return Sub( rhs ); }
		ND_ Self	operator * (Scalar_t rhs)				C_NE___	{ return Mul( rhs ); }

		ND_ Self	operator & (const Self &rhs)			C_NE___	{ return And( rhs ); }
		ND_ Self	operator | (const Self &rhs)			C_NE___	{ return Or( rhs ); }
		ND_ Self	operator ^ (const Self &rhs)			C_NE___	{ return Xor( rhs ); }

		ND_ Self	operator & (Scalar_t rhs)				C_NE___	{ return And( rhs ); }
		ND_ Self	operator | (Scalar_t rhs)				C_NE___	{ return Or( rhs ); }
		ND_ Self	operator ^ (Scalar_t rhs)				C_NE___	{ return Xor( rhs ); }

		ND_ Self	operator << (uint shift)				C_NE___	{ return LShift( shift ); }
		ND_ Self	operator << (const Shift64_t &shift)	C_NE___	{ return LShift( shift ); }
		ND_ Self	operator << (const Signed_t &shift)		C_NE___	{ return LShift( shift ); }

		ND_ Self	operator >> (uint shift)				C_NE___	{ return RShift( shift ); }
		ND_ Self	operator >> (const Shift64_t &shift)	C_NE___	{ return RShift( shift ); }
		ND_ Self	operator >> (const Signed_t &shift)		C_NE___	{ return RShift( shift ); }

		ND_ Bool_t	operator == (const Self &rhs)			C_NE___	{ return Equal( rhs ); }
		ND_ Bool_t	operator != (const Self &rhs)			C_NE___	{ return NotEqual( rhs ); }
		ND_ Bool_t	operator >  (const Self &rhs)			C_NE___	{ return Greater( rhs ); }
		ND_ Bool_t	operator <  (const Self &rhs)			C_NE___	{ return Less( rhs ); }
		ND_ Bool_t	operator >= (const Self &rhs)			C_NE___	{ return GEqual( rhs ); }
		ND_ Bool_t	operator <= (const Self &rhs)			C_NE___	{ return LEqual( rhs ); }
	  #endif // AVX2


		template <uint I> ND_ Scalar_t	get ()				C_NE___;
		template <uint I> ND_ Self		set (Scalar_t val)	C_NE___;

		ND_ Native_t &		Ref ()							__NE___	{ return _value; }
		ND_ Native_t const&	Ref ()							C_NE___	{ return _value; }

		ND_ friend Self  Select (const Bool_t &condition, const Self &ifTrue, const Self &ifFalse) __NE___ { return _Select( condition, ifTrue, ifFalse ); }


	  #if AE_SIMD_AVX >= 2
		template <typename T = Scalar_t> requires( IsSignedInteger<T> )
		ND_ Self	Negative ()								C_NE___	{ return Self{0}.Sub( *this ); }		// TODO: optimize ?

		template <typename T = Scalar_t> requires( IsSignedInteger<T> )
		ND_ Self	Abs ()									C_NE___;	// abs(x)

		ND_ Self	Add (const Scalar_t rhs)				C_NE___	{ return Add( Self{rhs} ); }
		ND_ Self	Add (const Self &rhs)					C_NE___;
		ND_ Self	AddSat (const Self &rhs)				C_NE___;

		ND_ Self	Sub (const Scalar_t rhs)				C_NE___	{ return Sub( Self{rhs} ); }
		ND_ Self	Sub (const Self &rhs)					C_NE___;
		ND_ Self	SubSat (const Self &rhs)				C_NE___;

		ND_ Self	Mul (const Scalar_t rhs)				C_NE___	{ return Mul( Self{rhs} ); }
		ND_ Self	Mul (const Self &rhs)					C_NE___;
		ND_ auto	MulExt (const Self &rhs)				C_NE___;

		ND_ Self	Min (const Self &rhs)					C_NE___;
		ND_ Self	Max (const Self &rhs)					C_NE___;

		ND_ Self	Min (const Scalar_t rhs)				C_NE___	{ return Min( Self{rhs} ); }
		ND_ Self	Max (const Scalar_t rhs)				C_NE___	{ return Max( Self{rhs} ); }

		ND_ Bool_t	Equal (const Self &rhs)					C_NE___;
		ND_ Bool_t	NotEqual (const Self &rhs)				C_NE___	{ return not Equal( rhs ); }
		ND_ Bool_t	Greater (const Self &rhs)				C_NE___;
		ND_ Bool_t	Less (const Self &rhs)					C_NE___	{ return rhs.Greater( *this ); }
		ND_ Bool_t	LEqual (const Self &rhs)				C_NE___;
		ND_ Bool_t	GEqual (const Self &rhs)				C_NE___	{ return rhs.LEqual( *this ); }

		ND_ Bool_t	IsZero ()								C_NE___	{ return Equal( Self{} ); }
		ND_ Bool_t	IsNotZero ()							C_NE___	{ return NotEqual( Self{} ); }

		// sum/max of all lanes, result written to all lanes
		ND_ Self		ReduceAdd ()						C_NE___;	// GLSL: subgroup.Add
		ND_ auto		ReduceAddExt ()						C_NE___;	// 32bit int will return 64bit int, etc
		ND_ Self		ReduceMax ()						C_NE___;	// GLSL: subgroup.Max
		ND_ Self		ReduceMin ()						C_NE___;	// GLSL: subgroup.Min

		// helper
		ND_ Scalar_t	ReduceAddScalar ()					C_NE___	{ return ReduceAdd().template get<0>(); }
		ND_ auto		ReduceAddExtScalar ()				C_NE___;
		ND_ Scalar_t	ReduceMaxScalar ()					C_NE___	{ return ReduceMax().template get<0>(); }
		ND_ Scalar_t	ReduceMinScalar ()					C_NE___	{ return ReduceMin().template get<0>(); }

		// set 'simd[lane] += simd[lane-1]'
		ND_ Self		InclusiveAdd ()						C_NE___;	// GLSL: subgroup.InclusiveAdd
		ND_ Self		ExclusiveAdd ()						C_NE___;	// GLSL: subgroup.ExclusiveAdd


		ND_ Mask_t	ToBitfield ()							C_NE___	{ return Mask_t{_mm256_movemask_epi8( _value )}; }

		template <typename T = Scalar_t> requires( IsUnsignedInteger<T> )
		ND_ bool	All ()									C_NE___	{ return _mm256_movemask_epi8( _value ) == -1; }

		template <typename T = Scalar_t> requires( IsUnsignedInteger<T> )
		ND_ bool	Any ()									C_NE___	{ return _mm256_movemask_epi8( _value ) != 0; }

		template <typename T = Scalar_t> requires( IsUnsignedInteger<T> )
		ND_ bool	None ()									C_NE___	{ return _mm256_movemask_epi8( _value ) == 0; }


	// bit shift //
		template <uint Shift>
		ND_ Self	LShift_Logic ()							C_NE___;
		ND_ Self	LShift_Logic (uint shift)				C_NE___;
		ND_ Self	LShift_Logic (const Shift64_t &shift)	C_NE___;

		template <uint Shift>
		ND_ Self	RShift_Logic ()							C_NE___;
		ND_ Self	RShift_Logic (uint shift)				C_NE___;
		ND_ Self	RShift_Logic (const Shift64_t &shift)	C_NE___;

		template <uint Shift>
		ND_ Self	LShift_Arith ()							C_NE___;
		ND_ Self	LShift_Arith (uint shift)				C_NE___;
		ND_ Self	LShift_Arith (const Shift64_t &shift)	C_NE___;

		template <uint Shift>
		ND_ Self	RShift_Arith ()							C_NE___;
		ND_ Self	RShift_Arith (uint shift)				C_NE___;
		ND_ Self	RShift_Arith (const Shift64_t &shift)	C_NE___;

		static constexpr bool	Has_VecShift = true;
		ND_ Self	LShift_LogicV (const Unsigned_t &shift)	C_NE___;
		ND_ Self	RShift_LogicV (const Unsigned_t &shift)	C_NE___;
		ND_ Self	RShift_ArithV (const Unsigned_t &shift)	C_NE___;

		template <uint Shift>	ND_ Self  LShift ()					C_NE___	{ return RShift_Arith< Shift >(); }
		template <uint Shift>	ND_ Self  RShift ()					C_NE___	{ return RShift_Arith< Shift >(); }
		template <typename T>	ND_ Self  LShift (const T &shift)	C_NE___;
		template <typename T>	ND_ Self  RShift (const T &shift)	C_NE___;


	// per bit operations //
		ND_ Self	BitInverse ()							C_NE___	{ return AndNot( Self{UMax} ); }

		ND_ Self	And (const Self &rhs)					C_NE___	{ return Self{ _mm256_and_si256( _value, rhs._value )}; }
		ND_ Self	Or  (const Self &rhs)					C_NE___	{ return Self{ _mm256_or_si256( _value, rhs._value )}; }
		ND_ Self	Xor (const Self &rhs)					C_NE___	{ return Self{ _mm256_xor_si256( _value, rhs._value )}; }
		ND_ Self	AndNot (const Self &rhs)				C_NE___	{ return Self{ _mm256_andnot_si256( _value, rhs._value )}; } // ~a & b

		ND_ Self	And (const Scalar_t rhs)				C_NE___	{ return And( Self{rhs} ); }
		ND_ Self	Or  (const Scalar_t rhs)				C_NE___	{ return Or(  Self{rhs} ); }
		ND_ Self	Xor (const Scalar_t rhs)				C_NE___	{ return Xor( Self{rhs} ); }
		ND_ Self	AndNot (const Scalar_t rhs)				C_NE___	{ return AndNot( Self{rhs} ); } // ~a & b


	// conversion //
		template <uint V0, uint V1, uint V2, uint V3,
				  uint V4, uint V5, uint V6, uint V7,
				  typename T=Scalar_t> requires( sizeof(T)==4 )
		ND_ Self	Swizzle ()								C_NE___;

		template <uint X, uint Y, uint Z, uint W,
				  typename T=Scalar_t> requires( sizeof(T)==8 )
		ND_ Self	Swizzle ()								C_NE___;

		template <uint V0, uint V1, uint V2, uint V3,
				  uint V4, uint V5, uint V6, uint V7,
				  typename T=Scalar_t> requires( sizeof(T)==4 )
		ND_ Self	Shuffle (const Self &v8)				C_NE___;

		template <uint X, uint Y, uint Z, uint W,
				  typename T=Scalar_t> requires( sizeof(T)==8 )
		ND_ Self	Shuffle (const Self &v4567)				C_NE___;

		template <uint Low, uint High>
		ND_ Self	SwizzleParts ()							C_NE___;

		template <uint I=0>	ND_ auto	ToShort ()			C_NE___;
		template <uint I=0>	ND_ auto	ToInt ()			C_NE___;
		template <uint I=0>	ND_ auto	ToLong ()			C_NE___;
	  #endif // AVX2


		ND_ Array_t	ToArray ()								C_NE___	{ Array_t arr;  ToArray( OUT arr.data() );  return arr; }
		void		ToArray (OUT Scalar_t* dst)				C_NE___ { NonNull( dst );  _mm256_storeu_si256( OUT reinterpret_cast<Native_t *>(dst), _value ); }
		void		ToAlignedArray (OUT Scalar_t* dst)		C_NE___	{ CheckPointerCast<Native_t>( dst );  _mm256_store_si256( OUT reinterpret_cast<Native_t *>(dst), _value ); }

		template <typename DstType>
		ND_ DstType	BitCast ()								C_NE___;

		template <typename DstScalar>
		ND_ auto	Convert ()								C_NE___;

		template <uint Idx>
		ND_ auto	Part ()									C_NE___	{ StaticAssert( Idx < parts );  return SimdTInt128<Scalar_t>{_mm256_extractf128_si256( _value, Idx )}; }

		template <typename T = Scalar_t> requires( IsUnsignedInteger<T> )
		ND_ auto	ToSigned ()								C_NE___;

		template <typename T = Scalar_t> requires( IsSignedInteger<T> )
		ND_ auto	ToUnsigned ()							C_NE___;

		template <uint I=0>	ND_ auto	ToFloat ()			C_NE___;
		template <uint I=0>	ND_ auto	ToDouble ()			C_NE___;

		template <typename T = Scalar_t> requires( sizeof(T)==8 )
		ND_ explicit operator PackedVec<T,count> ()			C_NE___
		{
			StaticAssert( IsSame< T, Scalar_t >);
			StaticAssert( count <= 4 );
			PackedVec<T,count> tmp;
			_mm256_storeu_si256( OUT reinterpret_cast<Native_t *>(&tmp.x), _value );
			return tmp;
		}


	// Features //
		NdCe__ static bool  Has_Arithmetic ()				{ return AE_SIMD_AVX >= 2; }
		NdCe__ static bool  Has_VecLShift_Logic ();
		NdCe__ static bool  Has_VecRShift_Logic ();
		NdCe__ static bool  Has_VecLShift_Arithmetic ()		{ return false; }
		NdCe__ static bool  Has_VecRShift_Arithmetic ();
		NdCe__ static bool  Has_ScalarShift_Logic ();
		NdCe__ static bool  Has_ScalarShift_Arithmetic ();
		NdCe__ static bool  Has_ReduceAdd ();
		NdCe__ static bool  Has_ReduceAddExt ();
		NdCe__ static bool  Has_ReduceMinMax ();
		NdCe__ static bool  Has_InclusiveAdd ();
		NdCe__ static bool  Has_Mul ();
		NdCe__ static bool  Has_MulExt ();
		NdCe__ static bool  Has_Div ()						{ return false; }
		NdCe__ static bool  Has_Equal ();
		NdCe__ static bool  Has_Greater ();
		NdCe__ static bool  _Has_Native_Greater ();
		NdCe__ static bool  Has_MinMax ();
		NdCe__ static bool  Has_Swizzle ()					{ return AE_SIMD_AVX >= 2 and sizeof(Scalar_t)>=4; }
		NdCe__ static bool  Has_Shuffle ()					{ return AE_SIMD_AVX >= 2 and sizeof(Scalar_t)>=4; }

		template <typename DstType>	NdCe__ static bool  Has_BitCast ();
		template <typename DstType>	NdCe__ static bool  Has_Convert ();

	private:
	  #if 0 //AE_SIMD_AVX >= 30 // ???
		template <typename T = Scalar_t> requires( IsSame<T,int> )
		ND_ SimdDouble8  _IntToDouble8 ()					C_NE___;

		template <typename T = Scalar_t> requires( IsSame<T,uint> )
		ND_ SimdDouble8  _UIntToDouble8 ()					C_NE___;

		template <typename T = Scalar_t> requires( IsSame<T,slong> )
		ND_ SimdDouble4  _LongToDouble4 ()					C_NE___;

		template <typename T = Scalar_t> requires( IsSame<T,ulong> )
		ND_ SimdDouble4  _ULongToDouble4 ()					C_NE___;
	  #endif

		ND_ static Self  _Select (const Bool_t &condition, const Self &ifTrue, const Self &ifFalse) __NE___;
	};
//-----------------------------------------------------------------------------

} // AE::Base
#endif // AE_SIMD_AVX >= 1
