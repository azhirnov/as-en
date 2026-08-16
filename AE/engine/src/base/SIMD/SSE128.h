// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	128 bit SIMD
*/

#pragma once

#if AE_SIMD_SSE >= 20

namespace AE::Base
{

	//
	// 128 bit float (SSE 2.x)
	//
	#define AE_SIMD_SimdFloat4
	struct SimdFloat4
	{
	// types
	public:
		static constexpr uint	count	= 4;
		static constexpr uint	parts	= 1;

		using Scalar_t		= float;
		using Self			= SimdFloat4;
		using Native_t		= __m128;
		using Array_t		= StaticArray< Scalar_t, count >;
		using Ptr_t			= AlignedPtr< sizeof(Native_t) >;
		using CPtr_t		= AlignedPtr< sizeof(Native_t), true >;
		using SimdInt_t		= SimdInt4;
		using SimdUInt_t	= SimdUInt4;
		using Mask_t		= MSBMask< count, 0 >;

		StaticAssert( sizeof(Array_t) == sizeof(Native_t) );


		struct Bool4
		{
		private:
			Native_t	_value;		// uint[4]

		public:
			explicit Bool4 (bool val)							__NE___	: _value{ val ? True()._value : False()._value } {}
			explicit Bool4 (Native_t val)						__NE___	: _value{ val } {}

			explicit Bool4 (__m128i val)						__NE___	: _value{ _mm_castsi128_ps( val )} {}
			explicit Bool4 (const SimdInt_t &v)					__NE___;
			explicit Bool4 (const SimdUInt_t &v)				__NE___;

			Bool4 (bool v0, bool v1, bool v2, bool v3)			__NE___;

			ND_ Bool4	operator | (const Bool4 &rhs)			C_NE___	{ return Bool4{ _mm_or_ps( _value, rhs._value )}; }
			ND_ Bool4	operator & (const Bool4 &rhs)			C_NE___	{ return Bool4{ _mm_and_ps( _value, rhs._value )}; }
			ND_ Bool4	operator ^ (const Bool4 &rhs)			C_NE___	{ return Bool4{ _mm_xor_ps( _value, rhs._value )}; }
			ND_ Bool4	operator ! ()							C_NE___	{ return Bool4{ _mm_andnot_ps( _value, True()._value )}; }

			ND_ Bool4	operator == (const Bool4 &rhs)			C_NE___	{ return Bool4{ _mm_and_ps( _value, rhs._value )}; }
			ND_ Bool4	operator != (const Bool4 &rhs)			C_NE___	{ return Bool4{ _mm_andnot_ps( _value, rhs._value )}; }

			ND_ Bool4	AndNot (const Bool4 &rhs)				C_NE___	{ return Bool4{ _mm_andnot_ps( _value, rhs._value )}; }		// ~a & b
			ND_ Self	AndNot (const Self &rhs)				C_NE___	{ return Self{ _mm_andnot_ps( _value, rhs.Ref() )}; }		// ~a & b

			template <uint I> ND_ bool	 get ()					C_NE___;
			template <uint I> ND_ Bool4  set (bool val)			C_NE___;

			ND_ bool	All ()									C_NE___	{ return _mm_movemask_ps( _value ) == 0xF; }
			ND_ bool	Any ()									C_NE___	{ return _mm_movemask_ps( _value ) != 0; }
			ND_ bool	None ()									C_NE___	{ return _mm_movemask_ps( _value ) == 0; }

			ND_ auto&	Ref ()									C_NE___	{ return _value; }

			ND_ Mask_t	ToBitfield ()							C_NE___	{ return Mask_t{_mm_movemask_ps( _value )}; }

			template <typename DstType>
			ND_ DstType	BitCast ()								C_NE___;

			ND_ static Bool4	True ()							__NE___	{ auto z = _mm_setzero_ps();  return Bool4{_mm_cmpeq_ps( z, z )}; }
			ND_ static Bool4	False ()						__NE___	{ return Bool4{_mm_setzero_ps()}; }
		};
		using Bool_t = Bool4;


	// variables
	private:
		Native_t	_value;		// float[4]


	// methods
	public:
		SimdFloat4 ()											__NE___	: _value{ _mm_setzero_ps() } {}
		SimdFloat4 (Zero_t)										__NE___	: _value{ _mm_setzero_ps() } {}
		explicit SimdFloat4 (Scalar_t v)						__NE___	: _value{ _mm_set1_ps( v )} {}
		explicit SimdFloat4 (const Native_t &v)					__NE___	: _value{ v } {}
		explicit SimdFloat4 (const Scalar_t* ptr)				__NE___	: _value{ _mm_loadu_ps( GetNonNull( ptr ))} {}
		explicit SimdFloat4 (const CPtr_t ptr)					__NE___	: _value{ _mm_load_ps( ptr.Cast<Scalar_t>() )} {}
		explicit SimdFloat4 (const Bool4 &v)					__NE___ : _value{ _mm_and_ps( v.Ref(), _mm_set1_ps(1.f) )} {}	// convert to 0.0 or 1.0
		SimdFloat4 (Scalar_t x, Scalar_t y, Scalar_t z, Scalar_t w)	__NE___	: _value{ _mm_set_ps( w, z, y, x )} {}

		ND_ Self	operator +  (const Self &rhs)				C_NE___	{ return Add( rhs ); }
		ND_ Self	operator -  (const Self &rhs)				C_NE___	{ return Sub( rhs ); }
		ND_ Self	operator *  (const Self &rhs)				C_NE___	{ return Mul( rhs ); }
		ND_ Self	operator /  (const Self &rhs)				C_NE___	{ return PreciseDiv( rhs ); }

		ND_ Self	operator +  (const Scalar_t rhs)			C_NE___	{ return Add( rhs ); }
		ND_ Self	operator -  (const Scalar_t rhs)			C_NE___	{ return Sub( rhs ); }
		ND_ Self	operator *  (const Scalar_t rhs)			C_NE___	{ return Mul( rhs ); }
		ND_ Self	operator /  (const Scalar_t rhs)			C_NE___	{ return PreciseDiv( rhs ); }

		ND_ Bool4	operator == (const Self &rhs)				C_NE___	{ return Equal( rhs ); }
		ND_ Bool4	operator != (const Self &rhs)				C_NE___	{ return NotEqual( rhs ); }
		ND_ Bool4	operator >  (const Self &rhs)				C_NE___	{ return Greater( rhs ); }
		ND_ Bool4	operator <  (const Self &rhs)				C_NE___	{ return Less( rhs ); }
		ND_ Bool4	operator >= (const Self &rhs)				C_NE___	{ return GEqual( rhs ); }
		ND_ Bool4	operator <= (const Self &rhs)				C_NE___	{ return LEqual( rhs ); }

		template <uint I> ND_ Scalar_t	get ()					C_NE___;
		template <uint I> ND_ Self		set (Scalar_t val)		C_NE___;

		ND_ Native_t &			Ref ()							__NE___	{ return _value; }
		ND_ Native_t const&		Ref ()							C_NE___	{ return _value; }

		ND_ Self	Add (const Self &rhs)						C_NE___	{ return Self{ _mm_add_ps( _value, rhs._value )}; }
		ND_ Self	Sub (const Self &rhs)						C_NE___	{ return Self{ _mm_sub_ps( _value, rhs._value )}; }
		ND_ Self	Mul (const Self &rhs)						C_NE___	{ return Self{ _mm_mul_ps( _value, rhs._value )}; }
		ND_ Self	PreciseDiv (const Self &rhs)				C_NE___	{ return Self{ _mm_div_ps( _value, rhs._value )}; }
		ND_ Self	Min (const Self &rhs)						C_NE___	{ return Self{ _mm_min_ps( _value, rhs._value )}; }
		ND_ Self	Max (const Self &rhs)						C_NE___	{ return Self{ _mm_max_ps( _value, rhs._value )}; }

		ND_ Self	Add (const Scalar_t rhs)					C_NE___	{ return Self{ _mm_add_ps( _value, _mm_set1_ps( rhs ))}; }
		ND_ Self	Sub (const Scalar_t rhs)					C_NE___	{ return Self{ _mm_sub_ps( _value, _mm_set1_ps( rhs ))}; }
		ND_ Self	Mul (const Scalar_t rhs)					C_NE___	{ return Self{ _mm_mul_ps( _value, _mm_set1_ps( rhs ))}; }
		ND_ Self	PreciseDiv (const Scalar_t rhs)				C_NE___	{ return Self{ _mm_div_ps( _value, _mm_set1_ps( rhs ))}; }
		ND_ Self	Min (const Scalar_t rhs)					C_NE___	{ return Self{ _mm_min_ps( _value, _mm_set1_ps( rhs ))}; }
		ND_ Self	Max (const Scalar_t rhs)					C_NE___	{ return Self{ _mm_max_ps( _value, _mm_set1_ps( rhs ))}; }

		// fused multiply add/sub (FMA) / fused negative multiply add/sub (FNMA)
	  #if AE_SIMD_FMA
		ND_ friend Self  FusedMulAdd    (const Self &a, const Self &b, const Self &c)	__NE___ { return Self{ _mm_fmadd_ps(    a._value, b._value, c._value )}; }	// a * b + c
		ND_ friend Self  FusedMulSub    (const Self &a, const Self &b, const Self &c)	__NE___ { return Self{ _mm_fmsub_ps(    a._value, b._value, c._value )}; }	// a * b - c
		ND_ friend Self  FusedMulAddSub (const Self &a, const Self &b, const Self &c)	__NE___ { return Self{ _mm_fmaddsub_ps( a._value, b._value, c._value )}; }	// { a0 * b0 - c0, a1 * b1 + c1, a2 * b2 - c2, a3 * b3 + c3 }
		ND_ friend Self  FusedMulSubAdd (const Self &a, const Self &b, const Self &c)	__NE___ { return Self{ _mm_fmaddsub_ps( a._value, b._value, c._value )}; }	// { a0 * b0 + c0, a1 * b1 - c1, a2 * b2 + c2, a3 * b3 - c3 }
		ND_ friend Self  FusedNegMulAdd (const Self &a, const Self &b, const Self &c)	__NE___ { return Self{ _mm_fnmadd_ps(   a._value, b._value, c._value )}; }	// -a * b + c
		ND_ friend Self  FusedNegMulSub (const Self &a, const Self &b, const Self &c)	__NE___ { return Self{ _mm_fnmsub_ps(   a._value, b._value, c._value )}; }	// -a * b - c
	  #endif

		ND_ friend Self  Lerp    (const Self &x, const Self &y, const Self &factor)						__NE___	{ return x * (Self{1.f} - factor) + y * factor; }
			friend Self  Select  (const Bool4 &condition, const Self &ifTrue, const Self &ifFalse)		__NE___;
		ND_ friend Self  SelectF (const Self &x, const Self &y, const Self &ifTrue, const Self &ifFalse)__NE___	{ return Lerp( ifFalse, ifTrue, x.LessF(y) ); }

		ND_ Self	Abs ()										C_NE___	{ return Self{ _mm_andnot_ps( _mm_set1_ps(-0.0f), _value )}; }
		ND_ Self	PreciseSqrt ()								C_NE___	{ return Self{ _mm_sqrt_ps( _value )}; }

		ND_ Self	Reciprocal ()								C_NE___	{ return Self{ _mm_rcp_ps( _value )}; }							// approx (1 / x)
		ND_ Self	FastInvSqrt ()								C_NE___	{ return Self{ _mm_rsqrt_ps( _value )}; }						// approx (1 / sqrt(x))
		ND_ Self	FastSqrt ()									C_NE___	{ return Self{ _mm_mul_ps( _value, _mm_rsqrt_ps( _value ))}; }	// approx (x / sqrt(x))
		ND_ Self	FastDiv (const Self &rhs)					C_NE___	{ return Self{ _mm_mul_ps( _value, _mm_rcp_ps( rhs._value ))}; }// approx (a / b)

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

		// operation with first element
		ND_ Self	Scalar_Add (const Self &rhs)				C_NE___	{ return Self{ _mm_add_ss( _value, rhs._value )}; }				// { a0 + b0,				a1, a2, a3 }
		ND_ Self	Scalar_Sub (const Self &rhs)				C_NE___	{ return Self{ _mm_sub_ss( _value, rhs._value )}; }				// { a0 - b0,				a1, a2, a3 }
		ND_ Self	Scalar_Mul (const Self &rhs)				C_NE___	{ return Self{ _mm_mul_ss( _value, rhs._value )}; }				// { a0 * b0,				a1, a2, a3 }
		ND_ Self	Scalar_Div (const Self &rhs)				C_NE___	{ return Self{ _mm_div_ss( _value, rhs._value )}; }				// { a0 / b0,				a1, a2, a3 }
		ND_ Self	Scalar_PreciseSqrt ()						C_NE___	{ return Self{ _mm_sqrt_ss( _value )}; }						// { sqrt(a0),				a1, a2, a3 }
		ND_ Self	Scalar_FastInvSqrt ()						C_NE___	{ return Self{ _mm_rsqrt_ss( _value )}; }						// { approx (1 / sqrt(a0)),	a1, a2, a3 }

		ND_ Bool4	Equal    (const Self &rhs)					C_NE___	{ return Bool4{ _mm_cmpeq_ps(  _value, rhs._value )}; }
		ND_ Bool4	NotEqual (const Self &rhs)					C_NE___	{ return Bool4{ _mm_cmpneq_ps( _value, rhs._value )}; }
		ND_ Bool4	Greater  (const Self &rhs)					C_NE___	{ return Bool4{ _mm_cmpgt_ps(  _value, rhs._value )}; }
		ND_ Bool4	Less     (const Self &rhs)					C_NE___	{ return Bool4{ _mm_cmplt_ps(  _value, rhs._value )}; }
		ND_ Bool4	GEqual   (const Self &rhs)					C_NE___	{ return Bool4{ _mm_cmpge_ps(  _value, rhs._value )}; }
		ND_ Bool4	LEqual   (const Self &rhs)					C_NE___	{ return Bool4{ _mm_cmple_ps(  _value, rhs._value )}; }

		ND_ Bool4	BitEqual (const Self&, EnabledBitCount acc)	C_NE___;	// TODO

		// compare and return 0.0 or 1.0
		ND_ Self	EqualF    (const Self &rhs)					C_NE___	{ return Self{ _mm_and_ps( _mm_cmpeq_ps(  _value, rhs._value ), _mm_set1_ps(1.f) )}; }
		ND_ Self	NotEqualF (const Self &rhs)					C_NE___	{ return Self{ _mm_and_ps( _mm_cmpneq_ps( _value, rhs._value ), _mm_set1_ps(1.f) )}; }
		ND_ Self	GreaterF  (const Self &rhs)					C_NE___	{ return Self{ _mm_and_ps( _mm_cmpgt_ps(  _value, rhs._value ), _mm_set1_ps(1.f) )}; }
		ND_ Self	LessF     (const Self &rhs)					C_NE___	{ return Self{ _mm_and_ps( _mm_cmplt_ps(  _value, rhs._value ), _mm_set1_ps(1.f) )}; }	// Step()
		ND_ Self	GEqualF   (const Self &rhs)					C_NE___	{ return Self{ _mm_and_ps( _mm_cmpge_ps(  _value, rhs._value ), _mm_set1_ps(1.f) )}; }
		ND_ Self	LEqualF   (const Self &rhs)					C_NE___	{ return Self{ _mm_and_ps( _mm_cmple_ps(  _value, rhs._value ), _mm_set1_ps(1.f) )}; }

		// compare only first element
		ND_ Bool4	Scalar_Equal    (const Self &rhs)			C_NE___	{ return Bool4{ _mm_cmpeq_ss(  _value, rhs._value )}; }
		ND_ Bool4	Scalar_NotEqual (const Self &rhs)			C_NE___	{ return Bool4{ _mm_cmpneq_ss( _value, rhs._value )}; }
		ND_ Bool4	Scalar_Greater  (const Self &rhs)			C_NE___	{ return Bool4{ _mm_cmpgt_ss(  _value, rhs._value )}; }
		ND_ Bool4	Scalar_Less     (const Self &rhs)			C_NE___	{ return Bool4{ _mm_cmplt_ss(  _value, rhs._value )}; }
		ND_ Bool4	Scalar_GEqual   (const Self &rhs)			C_NE___	{ return Bool4{ _mm_cmpge_ss(  _value, rhs._value )}; }
		ND_ Bool4	Scalar_LEqual   (const Self &rhs)			C_NE___	{ return Bool4{ _mm_cmple_ss(  _value, rhs._value )}; }

	  #if AE_SIMD_AVX >= 1	// AVX 1
		// ordered - comparison with NaN returns false
		ND_ Bool4	EqualO    (const Self &rhs)					C_NE___	{ return Bool4{ _mm_cmp_ps( _value, rhs._value, _CMP_EQ_OQ  )}; }
		ND_ Bool4	NotEqualO (const Self &rhs)					C_NE___	{ return Bool4{ _mm_cmp_ps( _value, rhs._value, _CMP_NEQ_OQ )}; }
		ND_ Bool4	GreaterO  (const Self &rhs)					C_NE___	{ return Bool4{ _mm_cmp_ps( _value, rhs._value, _CMP_GT_OQ  )}; }
		ND_ Bool4	LessO     (const Self &rhs)					C_NE___	{ return Bool4{ _mm_cmp_ps( _value, rhs._value, _CMP_LT_OQ  )}; }
		ND_ Bool4	GEqualO   (const Self &rhs)					C_NE___	{ return Bool4{ _mm_cmp_ps( _value, rhs._value, _CMP_GE_OQ  )}; }
		ND_ Bool4	LEqualO   (const Self &rhs)					C_NE___	{ return Bool4{ _mm_cmp_ps( _value, rhs._value, _CMP_LE_OQ  )}; }

		// unordered - comparison with NaN returns true
		ND_ Bool4	EqualU    (const Self &rhs)					C_NE___	{ return Bool4{ _mm_cmp_ps( _value, rhs._value, _CMP_EQ_UQ  )}; }
		ND_ Bool4	NotEqualU (const Self &rhs)					C_NE___	{ return Bool4{ _mm_cmp_ps( _value, rhs._value, _CMP_NEQ_UQ )}; }
		ND_ Bool4	GreaterU  (const Self &rhs)					C_NE___	{ return Bool4{ _mm_cmp_ps( _value, rhs._value, _CMP_NLE_UQ )}; }
		ND_ Bool4	LessU     (const Self &rhs)					C_NE___	{ return Bool4{ _mm_cmp_ps( _value, rhs._value, _CMP_NGE_UQ )}; }
		ND_ Bool4	GEqualU   (const Self &rhs)					C_NE___	{ return Bool4{ _mm_cmp_ps( _value, rhs._value, _CMP_NLT_UQ )}; }
		ND_ Bool4	LEqualU   (const Self &rhs)					C_NE___	{ return Bool4{ _mm_cmp_ps( _value, rhs._value, _CMP_NGT_UQ )}; }
	  #endif

	  #if defined(AE_COMPILER_MSVC) and not defined(AE_COMPILER_CLANG_CL) and AE_SIMD_AVX >= 1 // AVX & SVML library
		ND_ Self	Sin ()										C_NE___	{ return Self{ _mm_sin_ps( _value )}; }
		ND_ Self	Cos ()										C_NE___	{ return Self{ _mm_cos_ps( _value )}; }
		ND_ auto	SinCos ()									C_NE___	{ Native_t c, s = _mm_sincos_ps( OUT &c, _value );  return Tuple{s,c}; }
		ND_ Self	Tan ()										C_NE___	{ return Self{ _mm_tan_ps( _value )}; }
		ND_ Self	ASin ()										C_NE___	{ return Self{ _mm_asin_ps( _value )}; }
		ND_ Self	ACos ()										C_NE___	{ return Self{ _mm_acos_ps( _value )}; }
		ND_ Self	ATan ()										C_NE___	{ return Self{ _mm_atan_ps( _value )}; }
		ND_ Self	ATan2 (const Self &x)						C_NE___	{ return Self{ _mm_atan2_ps( _value, x._value )}; }

		// TODO: sind, cosd, tand, _mm_log1p_ps, _mm_logb_ps, _mm_expm1_ps ?
		ND_ Self	SinH ()										C_NE___	{ return Self{ _mm_sinh_ps( _value )}; }
		ND_ Self	CosH ()										C_NE___	{ return Self{ _mm_cosh_ps( _value )}; }
		ND_ Self	TanH ()										C_NE___	{ return Self{ _mm_tanh_ps( _value )}; }
		ND_ Self	ASinH ()									C_NE___	{ return Self{ _mm_asinh_ps( _value )}; }
		ND_ Self	ACosH ()									C_NE___	{ return Self{ _mm_acosh_ps( _value )}; }
		ND_ Self	ATanH ()									C_NE___	{ return Self{ _mm_atanh_ps( _value )}; }

		ND_ Self	Ln ()										C_NE___	{ return Self{ _mm_log_ps( _value )}; }
		ND_ Self	Log2 ()										C_NE___	{ return Self{ _mm_log2_ps( _value )}; }
		ND_ Self	Log10 ()									C_NE___	{ return Self{ _mm_log10_ps( _value )}; }

		ND_ Self	Exp ()										C_NE___	{ return Self{ _mm_exp_ps( _value )}; }
		ND_ Self	Exp2 ()										C_NE___	{ return Self{ _mm_exp2_ps( _value )}; }
		ND_ Self	Exp10 ()									C_NE___	{ return Self{ _mm_exp10_ps( _value )}; }

		ND_ Self	PreciseInvSqrt ()							C_NE___	{ return Self{ _mm_invsqrt_ps( _value )}; }
		ND_ Self	Cbrt ()										C_NE___	{ return Self{ _mm_cbrt_ps( _value )}; }
		ND_ Self	InvCbrt ()									C_NE___	{ return Self{ _mm_invcbrt_ps( _value )}; }

		ND_ Self	Mod (const Self &rhs)						C_NE___	{ return Self{ _mm_fmod_ps( _value, rhs._value )}; }
		ND_ Self	Pow (const Self &base)						C_NE___	{ return Self{ _mm_pow_ps( _value, base._value )}; }
	  #endif

	  #if AE_SIMD_SSE >= 41
		ND_ Self	Floor ()									C_NE___	{ return Self{ _mm_round_ps( _value, _MM_FROUND_FLOOR )}; }
		ND_ Self	Ceil ()										C_NE___	{ return Self{ _mm_round_ps( _value, _MM_FROUND_CEIL  )}; }
		ND_ Self	Trunc ()									C_NE___	{ return Self{ _mm_round_ps( _value, _MM_FROUND_TRUNC )}; }
		ND_ Self	Round ()									C_NE___	{ return Self{ _mm_round_ps( _value, _MM_FROUND_NINT  )}; }
		ND_ Self	RoundEven ()								C_NE___	{ return Self{ _mm_round_ps( _value, _MM_FROUND_RINT  )}; }
	  #endif


	// per bit operations //
		ND_ Self	BitInverse ()								C_NE___	{ return AndNot( Bool4::True() ); }

		ND_ Self	And (const Self &rhs)						C_NE___	{ return Self{ _mm_and_ps( _value, rhs._value )}; }
		ND_ Self	Or  (const Self &rhs)						C_NE___	{ return Self{ _mm_or_ps( _value, rhs._value )}; }
		ND_ Self	Xor (const Self &rhs)						C_NE___	{ return Self{ _mm_xor_ps( _value, rhs._value )}; }
		ND_ Self	AndNot (const Self &rhs)					C_NE___	{ return Self{ _mm_andnot_ps( _value, rhs._value )}; }		// ~a & b

		ND_ Self	And (const Bool4 &rhs)						C_NE___	{ return Self{ _mm_and_ps( _value, rhs.Ref() )}; }
		ND_ Self	Or  (const Bool4 &rhs)						C_NE___	{ return Self{ _mm_or_ps( _value, rhs.Ref() )}; }
		ND_ Self	Xor (const Bool4 &rhs)						C_NE___	{ return Self{ _mm_xor_ps( _value, rhs.Ref() )}; }
		ND_ Self	AndNot (const Bool4 &rhs)					C_NE___	{ return Self{ _mm_andnot_ps( _value, rhs.Ref() )}; }		// ~a & b

		ND_ Self	And (const uint rhs)						C_NE___	{ return And( Self{Base::BitCast<Scalar_t>( rhs )} ); }
		ND_ Self	Or  (const uint rhs)						C_NE___	{ return Or(  Self{Base::BitCast<Scalar_t>( rhs )} ); }
		ND_ Self	Xor (const uint rhs)						C_NE___	{ return Xor( Self{Base::BitCast<Scalar_t>( rhs )} ); }
		ND_ Self	AndNot (const uint rhs)						C_NE___	{ return AndNot( Self{Base::BitCast<Scalar_t>( rhs )} ); }	// ~a & b


	// conversion //
		template <uint Idx>
		ND_ Self	Part ()										C_NE___ { StaticAssert( Idx < parts );  return *this; }

		template <uint X, uint Y, uint Z, uint W>
		ND_ Self	Swizzle ()									C_NE___;

		template <uint X, uint Y, uint Z, uint W>
		ND_ Self	Shuffle (const Self &v4567)					C_NE___;

		ND_ Array_t	ToArray ()									C_NE___	{ Array_t arr;  _mm_storeu_ps( OUT arr.data(), _value );  return arr; }
			void	ToArray (OUT Scalar_t* dst)					C_NE___	{ NonNull( dst );  _mm_storeu_ps( OUT dst, _value ); }
			void	ToAlignedArray (OUT Ptr_t dst)				C_NE___	{ _mm_store_ps( OUT dst.Cast<Scalar_t>(), _value ); }

		ND_ explicit operator packed_float4 ()					C_NE___	{ packed_float4 tmp;  _mm_storeu_ps( OUT &tmp.x, _value );  return tmp; }

		template <typename DstType>
		ND_ DstType	BitCast ()									C_NE___;

		template <typename DstScalar>
		ND_ auto	Convert ()									C_NE___;

		template <uint Idx>
		ND_ SimdDouble2	ToDouble ()								C_NE___;
		ND_ SimdDouble4	ToDouble4 ()							C_NE___;	// AVX

		ND_ SimdInt4	ToInt ()								C_NE___;
		ND_ SimdHalf8	ToHalf ()								C_NE___;


	// Features //
		NdCe__ static bool  Has_Arithmetic ()					{ return true; }
		NdCe__ static bool  Has_Equal ()						{ return true; }
		NdCe__ static bool  Has_Greater ()						{ return true; }
		NdCe__ static bool  Has_Rounding ()						{ return AE_SIMD_SSE >= 41; }
		NdCe__ static bool  Has_FusedMulAdd ()					{ return AE_SIMD_FMA > 0; }
		NdCe__ static bool  Has_MulAdd ()						{ return false; }
		NdCe__ static bool  Has_PreciseSqrt ()					{ return true; }
		NdCe__ static bool  Has_PreciseDiv ()					{ return true; }
		NdCe__ static bool  Has_ApproxReciprocal ()				{ return true; }
		NdCe__ static bool  Has_ApproxInvSqrt ()				{ return true; }
		NdCe__ static bool  Has_ReduceAdd ()					{ return Has_Swizzle(); }
		NdCe__ static bool  Has_ReduceMinMax ()					{ return Has_Swizzle(); }
		NdCe__ static bool  Has_InclusiveAdd ()					{ return true; }
		NdCe__ static bool  Has_BitEqual ()						{ return false; }
		NdCe__ static bool  Has_Swizzle ()						{ return true; }
		NdCe__ static bool  Has_Shuffle ()						{ return true; }

	  #if defined(AE_COMPILER_MSVC) and not defined(AE_COMPILER_CLANG_CL) and AE_SIMD_AVX >= 1 // AVX & SVML library
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
	  #if AE_SIMD_SSE >= 30
		// different behaviour for Float4 & Double4, keep private
		ND_ Self	HAdd (const Self &rhs)						C_NE___	{ return Self{ _mm_hadd_ps( _value, rhs._value )}; }		// { a0 + a1, a2 + a3, b0 + b1, b2 + b3 }
		ND_ Self	HSub (const Self &rhs)						C_NE___	{ return Self{ _mm_hsub_ps( _value, rhs._value )}; }		// { a0 - a1, a2 - a3, b0 - b1, b2 - b3 }
		ND_ Self	AddSub (const Self &rhs)					C_NE___	{ return Self{ _mm_addsub_ps( _value, rhs._value )}; }		// { a0 - b0, a1 + b1, a2 - b2, a3 + b3 }
	  #endif
	};



	//
	// 128 bit double (SSE 2.x)
	//
	#define AE_SIMD_SimdDouble2
	struct SimdDouble2
	{
	// types
	public:
		static constexpr uint	count	= 2;
		static constexpr uint	parts	= 1;

		using Scalar_t		= double;
		using Self			= SimdDouble2;
		using Native_t		= __m128d;
		using Array_t		= StaticArray< Scalar_t, count >;
		using Ptr_t			= AlignedPtr< sizeof(Native_t) >;
		using CPtr_t		= AlignedPtr< sizeof(Native_t), true >;
		using SimdInt_t		= SimdLong2;
		using SimdUInt_t	= SimdULong2;
		using Mask_t		= MSBMask< count, 0 >;

		StaticAssert( sizeof(Array_t) == sizeof(Native_t) );


		struct Bool2
		{
		private:
			Native_t	_value;		// ulong[2]

		public:
			explicit Bool2 (bool val)							__NE___	: _value{ val ? True()._value : False()._value } {}
			explicit Bool2 (Native_t val)						__NE___	: _value{ val } {}

			explicit Bool2 (__m128i val)						__NE___	: _value{ _mm_castsi128_pd( val )} {}
			explicit Bool2 (const SimdInt_t &v)					__NE___;
			explicit Bool2 (const SimdUInt_t &v)				__NE___;

			Bool2 (bool v0, bool v1)							__NE___;

			ND_ Bool2	operator | (const Bool2 &rhs)			C_NE___	{ return Bool2{ _mm_or_pd( _value, rhs._value )}; }
			ND_ Bool2	operator & (const Bool2 &rhs)			C_NE___	{ return Bool2{ _mm_and_pd( _value, rhs._value )}; }
			ND_ Bool2	operator ^ (const Bool2 &rhs)			C_NE___	{ return Bool2{ _mm_xor_pd( _value, rhs._value )}; }
			ND_ Bool2	operator ! ()							C_NE___	{ return Bool2{ _mm_andnot_pd( _value, True()._value )}; }

			ND_ Bool2	operator == (const Bool2 &rhs)			C_NE___	{ return Bool2{ _mm_and_pd( _value, rhs._value )}; }
			ND_ Bool2	operator != (const Bool2 &rhs)			C_NE___	{ return Bool2{ _mm_andnot_pd( _value, rhs._value )}; }

			ND_ Bool2	AndNot (const Bool2 &rhs)				C_NE___	{ return Bool2{ _mm_andnot_pd( _value, rhs._value )}; }		// ~a & b
			ND_ Self	AndNot (const Self &rhs)				C_NE___	{ return Self{ _mm_andnot_pd( _value, rhs.Ref() )}; }		// ~a & b

			ND_ bool	All ()									C_NE___	{ return _mm_movemask_pd( _value ) == 0x3; }
			ND_ bool	Any ()									C_NE___	{ return _mm_movemask_pd( _value ) != 0; }
			ND_ bool	None ()									C_NE___	{ return _mm_movemask_pd( _value ) == 0; }

			ND_ auto&	Ref ()									C_NE___	{ return _value; }

			ND_ Mask_t	ToBitfield ()							C_NE___	{ return Mask_t{_mm_movemask_pd( _value )}; }

			template <typename DstType>
			ND_ DstType	BitCast ()								C_NE___;

			ND_ static Bool2	True ()							__NE___	{ auto z = _mm_setzero_pd();  return Bool2{_mm_cmpeq_pd( z, z )}; }
			ND_ static Bool2	False ()						__NE___	{ return Bool2{_mm_setzero_pd()}; }
		};
		using Bool_t = Bool2;


	// variables
	private:
		Native_t	_value;		// double[2]


	// methods
	public:
		SimdDouble2 ()											__NE___	: _value{ _mm_setzero_pd() } {}
		SimdDouble2 (Zero_t)									__NE___	: _value{ _mm_setzero_pd() } {}
		explicit SimdDouble2 (Scalar_t v)						__NE___	: _value{ _mm_set1_pd( v )} {}
		explicit SimdDouble2 (const Native_t &v)				__NE___	: _value{ v } {}
		explicit SimdDouble2 (const Scalar_t* ptr)				__NE___	: _value{ _mm_loadu_pd( GetNonNull( ptr ))} {}
		explicit SimdDouble2 (const CPtr_t ptr)					__NE___	: _value{ _mm_load_pd( ptr.Cast<Scalar_t>() )} {}
		explicit SimdDouble2 (const Bool2 &v)					__NE___ : _value{ _mm_and_pd( v.Ref(), _mm_set1_pd(1.0) )} {}	// convert to 0.0 or 1.0
		SimdDouble2 (Scalar_t x, Scalar_t y)					__NE___	: _value{ _mm_set_pd( y, x )} {}

		ND_ Self	operator +  (const Self &rhs)				C_NE___	{ return Add( rhs ); }
		ND_ Self	operator -  (const Self &rhs)				C_NE___	{ return Sub( rhs ); }
		ND_ Self	operator *  (const Self &rhs)				C_NE___	{ return Mul( rhs ); }
		ND_ Self	operator /  (const Self &rhs)				C_NE___	{ return PreciseDiv( rhs ); }

		ND_ Self	operator +  (const Scalar_t rhs)			C_NE___	{ return Add( rhs ); }
		ND_ Self	operator -  (const Scalar_t rhs)			C_NE___	{ return Sub( rhs ); }
		ND_ Self	operator *  (const Scalar_t rhs)			C_NE___	{ return Mul( rhs ); }
		ND_ Self	operator /  (const Scalar_t rhs)			C_NE___	{ return PreciseDiv( rhs ); }

		ND_ Bool2	operator == (const Self &rhs)				C_NE___	{ return Equal( rhs ); }
		ND_ Bool2	operator != (const Self &rhs)				C_NE___	{ return NotEqual( rhs ); }
		ND_ Bool2	operator >  (const Self &rhs)				C_NE___	{ return Greater( rhs ); }
		ND_ Bool2	operator <  (const Self &rhs)				C_NE___	{ return Less( rhs ); }
		ND_ Bool2	operator >= (const Self &rhs)				C_NE___	{ return GEqual( rhs ); }
		ND_ Bool2	operator <= (const Self &rhs)				C_NE___	{ return LEqual( rhs ); }

		template <uint I> ND_ Scalar_t	get ()					C_NE___;
		template <uint I> ND_ Self		set (Scalar_t val)		C_NE___;

		ND_ Native_t &			Ref ()							__NE___	{ return _value; }
		ND_ Native_t const&		Ref ()							C_NE___	{ return _value; }

		ND_ Self	Add (const Self &rhs)						C_NE___	{ return Self{ _mm_add_pd( _value, rhs._value )}; }
		ND_ Self	Sub (const Self &rhs)						C_NE___	{ return Self{ _mm_sub_pd( _value, rhs._value )}; }
		ND_ Self	Mul (const Self &rhs)						C_NE___	{ return Self{ _mm_mul_pd( _value, rhs._value )}; }
		ND_ Self	PreciseDiv (const Self &rhs)				C_NE___	{ return Self{ _mm_div_pd( _value, rhs._value )}; }
		ND_ Self	Min (const Self &rhs)						C_NE___	{ return Self{ _mm_min_pd( _value, rhs._value )}; }
		ND_ Self	Max (const Self &rhs)						C_NE___	{ return Self{ _mm_max_pd( _value, rhs._value )}; }

		ND_ Self	Add (const Scalar_t rhs)					C_NE___	{ return Self{ _mm_add_pd( _value, _mm_set1_pd( rhs ))}; }
		ND_ Self	Sub (const Scalar_t rhs)					C_NE___	{ return Self{ _mm_sub_pd( _value, _mm_set1_pd( rhs ))}; }
		ND_ Self	Mul (const Scalar_t rhs)					C_NE___	{ return Self{ _mm_mul_pd( _value, _mm_set1_pd( rhs ))}; }
		ND_ Self	PreciseDiv (const Scalar_t rhs)				C_NE___	{ return Self{ _mm_div_pd( _value, _mm_set1_pd( rhs ))}; }
		ND_ Self	Min (const Scalar_t rhs)					C_NE___	{ return Self{ _mm_min_pd( _value, _mm_set1_pd( rhs ))}; }
		ND_ Self	Max (const Scalar_t rhs)					C_NE___	{ return Self{ _mm_max_pd( _value, _mm_set1_pd( rhs ))}; }

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

		ND_ Bool2	Equal    (const Self &rhs)					C_NE___	{ return Bool2{ _mm_cmpeq_pd(  _value, rhs._value )}; }
		ND_ Bool2	NotEqual (const Self &rhs)					C_NE___	{ return Bool2{ _mm_cmpneq_pd( _value, rhs._value )}; }
		ND_ Bool2	Greater  (const Self &rhs)					C_NE___	{ return Bool2{ _mm_cmpgt_pd(  _value, rhs._value )}; }
		ND_ Bool2	Less     (const Self &rhs)					C_NE___	{ return Bool2{ _mm_cmplt_pd(  _value, rhs._value )}; }
		ND_ Bool2	GEqual   (const Self &rhs)					C_NE___	{ return Bool2{ _mm_cmpge_pd(  _value, rhs._value )}; }
		ND_ Bool2	LEqual   (const Self &rhs)					C_NE___	{ return Bool2{ _mm_cmple_pd(  _value, rhs._value )}; }

		ND_ Bool2	BitEqual (const Self&, EnabledBitCount acc)	C_NE___;	// TODO

		// compare only first element
		ND_ Bool2	Scalar_Equal    (const Self &rhs)			C_NE___	{ return Bool2{ _mm_cmpeq_sd(  _value, rhs._value )}; }
		ND_ Bool2	Scalar_NotEqual (const Self &rhs)			C_NE___	{ return Bool2{ _mm_cmpneq_sd( _value, rhs._value )}; }
		ND_ Bool2	Scalar_Greater  (const Self &rhs)			C_NE___	{ return Bool2{ _mm_cmpgt_sd(  _value, rhs._value )}; }
		ND_ Bool2	Scalar_Less     (const Self &rhs)			C_NE___	{ return Bool2{ _mm_cmplt_sd(  _value, rhs._value )}; }
		ND_ Bool2	Scalar_GEqual   (const Self &rhs)			C_NE___	{ return Bool2{ _mm_cmpge_sd(  _value, rhs._value )}; }
		ND_ Bool2	Scalar_LEqual   (const Self &rhs)			C_NE___	{ return Bool2{ _mm_cmple_sd(  _value, rhs._value )}; }

		// compare and return 0.0 or 1.0
		ND_ Self	EqualF    (const Self &rhs)					C_NE___	{ return Self{ _mm_and_pd( _mm_cmpeq_pd(  _value, rhs._value ), _mm_set1_pd(1.0) )}; }
		ND_ Self	NotEqualF (const Self &rhs)					C_NE___	{ return Self{ _mm_and_pd( _mm_cmpneq_pd( _value, rhs._value ), _mm_set1_pd(1.0) )}; }
		ND_ Self	GreaterF  (const Self &rhs)					C_NE___	{ return Self{ _mm_and_pd( _mm_cmpgt_pd(  _value, rhs._value ), _mm_set1_pd(1.0) )}; }
		ND_ Self	LessF     (const Self &rhs)					C_NE___	{ return Self{ _mm_and_pd( _mm_cmplt_pd(  _value, rhs._value ), _mm_set1_pd(1.0) )}; }	// Step()
		ND_ Self	GEqualF   (const Self &rhs)					C_NE___	{ return Self{ _mm_and_pd( _mm_cmpge_pd(  _value, rhs._value ), _mm_set1_pd(1.0) )}; }
		ND_ Self	LEqualF   (const Self &rhs)					C_NE___	{ return Self{ _mm_and_pd( _mm_cmple_pd(  _value, rhs._value ), _mm_set1_pd(1.0) )}; }

	  #if AE_SIMD_AVX >= 1	// AVX 1
		// ordered - comparison with NaN returns false
		ND_ Bool2	EqualO    (const Self &rhs)					C_NE___	{ return Bool2{ _mm_cmp_pd( _value, rhs._value, _CMP_EQ_OQ  )}; }
		ND_ Bool2	NotEqualO (const Self &rhs)					C_NE___	{ return Bool2{ _mm_cmp_pd( _value, rhs._value, _CMP_NEQ_OQ )}; }
		ND_ Bool2	GreaterO  (const Self &rhs)					C_NE___	{ return Bool2{ _mm_cmp_pd( _value, rhs._value, _CMP_GT_OQ  )}; }
		ND_ Bool2	LessO     (const Self &rhs)					C_NE___	{ return Bool2{ _mm_cmp_pd( _value, rhs._value, _CMP_LT_OQ  )}; }
		ND_ Bool2	GEqualO   (const Self &rhs)					C_NE___	{ return Bool2{ _mm_cmp_pd( _value, rhs._value, _CMP_GE_OQ  )}; }
		ND_ Bool2	LEqualO   (const Self &rhs)					C_NE___	{ return Bool2{ _mm_cmp_pd( _value, rhs._value, _CMP_LE_OQ  )}; }

		// unordered - comparison with NaN returns true
		ND_ Bool2	EqualU    (const Self &rhs)					C_NE___	{ return Bool2{ _mm_cmp_pd( _value, rhs._value, _CMP_EQ_UQ  )}; }
		ND_ Bool2	NotEqualU (const Self &rhs)					C_NE___	{ return Bool2{ _mm_cmp_pd( _value, rhs._value, _CMP_NEQ_UQ )}; }
		ND_ Bool2	GreaterU  (const Self &rhs)					C_NE___	{ return Bool2{ _mm_cmp_pd( _value, rhs._value, _CMP_NLE_UQ )}; }
		ND_ Bool2	LessU     (const Self &rhs)					C_NE___	{ return Bool2{ _mm_cmp_pd( _value, rhs._value, _CMP_NGE_UQ )}; }
		ND_ Bool2	GEqualU   (const Self &rhs)					C_NE___	{ return Bool2{ _mm_cmp_pd( _value, rhs._value, _CMP_NLT_UQ )}; }
		ND_ Bool2	LEqualU   (const Self &rhs)					C_NE___	{ return Bool2{ _mm_cmp_pd( _value, rhs._value, _CMP_NGT_UQ )}; }
	  #endif

	  #if defined(AE_COMPILER_MSVC) and not defined(AE_COMPILER_CLANG_CL) and AE_SIMD_AVX >= 1 // AVX & SVML library
		ND_ Self	Sin ()										C_NE___	{ return Self{ _mm_sin_pd( _value )}; }
		ND_ Self	Cos ()										C_NE___	{ return Self{ _mm_cos_pd( _value )}; }
		ND_ auto	SinCos ()									C_NE___	{ Native_t c, s = _mm_sincos_pd( OUT &c, _value );  return Tuple{s,c}; }
		ND_ Self	Tan ()										C_NE___	{ return Self{ _mm_tan_pd( _value )}; }
		ND_ Self	ASin ()										C_NE___	{ return Self{ _mm_asin_pd( _value )}; }
		ND_ Self	ACos ()										C_NE___	{ return Self{ _mm_acos_pd( _value )}; }
		ND_ Self	ATan ()										C_NE___	{ return Self{ _mm_atan_pd( _value )}; }
		ND_ Self	ATan2 (const Self &x)						C_NE___	{ return Self{ _mm_atan2_pd( _value, x._value )}; }

		ND_ Self	SinH ()										C_NE___	{ return Self{ _mm_sinh_pd( _value )}; }
		ND_ Self	CosH ()										C_NE___	{ return Self{ _mm_cosh_pd( _value )}; }
		ND_ Self	TanH ()										C_NE___	{ return Self{ _mm_tanh_pd( _value )}; }
		ND_ Self	ASinH ()									C_NE___	{ return Self{ _mm_asinh_pd( _value )}; }
		ND_ Self	ACosH ()									C_NE___	{ return Self{ _mm_acosh_pd( _value )}; }
		ND_ Self	ATanH ()									C_NE___	{ return Self{ _mm_atanh_pd( _value )}; }

		ND_ Self	Ln ()										C_NE___	{ return Self{ _mm_log_pd( _value )}; }
		ND_ Self	Log2 ()										C_NE___	{ return Self{ _mm_log2_pd( _value )}; }
		ND_ Self	Log10 ()									C_NE___	{ return Self{ _mm_log10_pd( _value )}; }

		ND_ Self	Exp ()										C_NE___	{ return Self{ _mm_exp_pd( _value )}; }
		ND_ Self	Exp2 ()										C_NE___	{ return Self{ _mm_exp2_pd( _value )}; }
		ND_ Self	Exp10 ()									C_NE___	{ return Self{ _mm_exp10_pd( _value )}; }

		ND_ Self	PreciseInvSqrt ()							C_NE___	{ return Self{ _mm_invsqrt_pd( _value )}; }
		ND_ Self	Cbrt ()										C_NE___	{ return Self{ _mm_cbrt_pd( _value )}; }
		ND_ Self	InvCbrt ()									C_NE___	{ return Self{ _mm_invcbrt_pd( _value )}; }

		ND_ Self	Mod (const Self &rhs)						C_NE___	{ return Self{ _mm_fmod_pd( _value, rhs._value )}; }
		ND_ Self	Pow (const Self &base)						C_NE___	{ return Self{ _mm_pow_pd( _value, base._value )}; }
	  #endif

	  #if AE_SIMD_SSE >= 41
		ND_ Self	Floor ()									C_NE___	{ return Self{ _mm_round_pd( _value, _MM_FROUND_FLOOR )}; }
		ND_ Self	Ceil ()										C_NE___	{ return Self{ _mm_round_pd( _value, _MM_FROUND_CEIL  )}; }
		ND_ Self	Trunc ()									C_NE___	{ return Self{ _mm_round_pd( _value, _MM_FROUND_TRUNC )}; }
		ND_ Self	Round ()									C_NE___	{ return Self{ _mm_round_pd( _value, _MM_FROUND_NINT  )}; }
		ND_ Self	RoundEven ()								C_NE___	{ return Self{ _mm_round_pd( _value, _MM_FROUND_RINT  )}; }
	  #endif

		ND_ Self	Abs ()										C_NE___	{ return Self{ _mm_andnot_pd( _mm_set1_pd( -0.0 ), _value )}; }
		ND_ Self	PreciseSqrt ()								C_NE___	{ return Self{ _mm_sqrt_pd( _value )}; }

		ND_ Self	Reciprocal_fp32 ()							C_NE___;
	  #if AE_SIMD_AVX >= 31	// AVX512VL
		ND_ Self	Reciprocal ()								C_NE___	{ return Self{ _mm_rcp14_pd( _value )}; }	// approx (1 / x)
	  #else
		ND_ Self	Reciprocal ()								C_NE___	{ return Reciprocal_fp32(); }
	  #endif
		ND_ Self	FastDiv (const Self &rhs)					C_NE___	{ return *this * rhs.Reciprocal(); }		// approx (a / b)


		// fused multiply (FM) / fused negative multiply (FNM)
	  #if AE_SIMD_FMA
		ND_ friend Self  FusedMulAdd    (const Self &a, const Self &b, const Self &c)	__NE___ { return Self{ _mm_fmadd_pd(    a._value, b._value, c._value )}; }	// a * b + c
		ND_ friend Self  FusedMulSub    (const Self &a, const Self &b, const Self &c)	__NE___ { return Self{ _mm_fmsub_pd(    a._value, b._value, c._value )}; }	// a * b - c
		ND_ friend Self  FusedMulAddSub (const Self &a, const Self &b, const Self &c)	__NE___ { return Self{ _mm_fmaddsub_pd( a._value, b._value, c._value )}; }	// { a0 * b0 - c0, a1 * b1 + c1 }
		ND_ friend Self  FusedMulSubAdd (const Self &a, const Self &b, const Self &c)	__NE___ { return Self{ _mm_fmaddsub_pd( a._value, b._value, c._value )}; }	// { a0 * b0 + c0, a1 * b1 - c1 }
		ND_ friend Self  FusedNegMulAdd (const Self &a, const Self &b, const Self &c)	__NE___ { return Self{ _mm_fnmadd_pd(   a._value, b._value, c._value )}; }	// -a * b + c
		ND_ friend Self  FusedNegMulSub (const Self &a, const Self &b, const Self &c)	__NE___ { return Self{ _mm_fnmsub_pd(   a._value, b._value, c._value )}; }	// -a * b - c
	  #endif

		ND_ friend Self  Lerp    (const Self &x, const Self &y, const Self &factor)						__NE___	{ return x * (Self{1.0} - factor) + y * factor; }
			friend Self  Select  (const Bool2 &condition, const Self &ifTrue, const Self &ifFalse)		__NE___;
		ND_ friend Self  SelectF (const Self &x, const Self &y, const Self &ifTrue, const Self &ifFalse)__NE___	{ return Lerp( ifFalse, ifTrue, x.LessF(y) ); }


	// per bit operations //
		ND_ Self	BitInverse ()								C_NE___	{ return AndNot( Bool2::True() ); }

		ND_ Self	And (const Self &rhs)						C_NE___	{ return Self{ _mm_and_pd( _value, rhs._value )}; }
		ND_ Self	Or  (const Self &rhs)						C_NE___	{ return Self{ _mm_or_pd( _value, rhs._value )}; }
		ND_ Self	Xor (const Self &rhs)						C_NE___	{ return Self{ _mm_xor_pd( _value, rhs._value )}; }
		ND_ Self	AndNot (const Self &rhs)					C_NE___	{ return Self{ _mm_andnot_pd( _value, rhs._value )}; }		// ~a & b

		ND_ Self	And (const Bool2 &rhs)						C_NE___	{ return Self{ _mm_and_pd( _value, rhs.Ref() )}; }
		ND_ Self	Or  (const Bool2 &rhs)						C_NE___	{ return Self{ _mm_or_pd( _value, rhs.Ref() )}; }
		ND_ Self	Xor (const Bool2 &rhs)						C_NE___	{ return Self{ _mm_xor_pd( _value, rhs.Ref() )}; }
		ND_ Self	AndNot (const Bool2 &rhs)					C_NE___	{ return Self{ _mm_andnot_pd( _value, rhs.Ref() )}; }		// ~a & b

		ND_ Self	And (const ulong rhs)						C_NE___	{ return And( Self{Base::BitCast<Scalar_t>( rhs )} ); }
		ND_ Self	Or  (const ulong rhs)						C_NE___	{ return Or(  Self{Base::BitCast<Scalar_t>( rhs )} ); }
		ND_ Self	Xor (const ulong rhs)						C_NE___	{ return Xor( Self{Base::BitCast<Scalar_t>( rhs )} ); }
		ND_ Self	AndNot (const ulong rhs)					C_NE___	{ return AndNot( Self{Base::BitCast<Scalar_t>( rhs )} ); }	// ~a & b


	// conversion //
		template <uint Idx>
		ND_ Self	Part ()										C_NE___ { StaticAssert( Idx < parts );  return *this; }

		template <uint X, uint Y>
		ND_ Self	Swizzle ()									C_NE___;

		template <uint X, uint Y>
		ND_ Self	Shuffle (const Self &v23)					C_NE___;

		ND_ Array_t	ToArray ()									C_NE___	{ Array_t arr;  _mm_storeu_pd( OUT arr.data(), _value );  return arr; }
			void	ToArray (OUT Scalar_t* dst)					C_NE___	{ NonNull( dst );  _mm_storeu_pd( OUT dst, _value ); }
			void	ToAlignedArray (OUT Ptr_t dst)				C_NE___	{ _mm_store_pd( OUT dst.Cast<Scalar_t>(), _value ); }

		ND_ explicit operator packed_double2 ()					C_NE___	{ packed_double2 tmp;  _mm_storeu_pd( OUT &tmp.x, _value );  return tmp; }

		ND_ SimdFloat4	ToFloat ()								C_NE___;
		ND_ SimdLong2	ToLong ()								C_NE___;	// AVX512
		ND_ SimdInt4	ToInt ()								C_NE___;

		template <typename DstType>
		ND_ DstType	BitCast ()									C_NE___;

		template <typename DstScalar>
		ND_ auto	Convert ()									C_NE___;


	// Features //
		NdCe__ static bool  Has_Arithmetic ()					{ return true; }
		NdCe__ static bool  Has_Equal ()						{ return true; }
		NdCe__ static bool  Has_Greater ()						{ return true; }
		NdCe__ static bool  Has_Rounding ()						{ return AE_SIMD_SSE >= 41; }
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

	  #if defined(AE_COMPILER_MSVC) and not defined(AE_COMPILER_CLANG_CL) and AE_SIMD_AVX >= 1 // AVX & SVML library
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
	  #if AE_SIMD_SSE >= 30
		// different behavior for Float4 & Double4, keep private
		ND_ Self	HAdd (const Self &rhs)						C_NE___	{ return Self{ _mm_hadd_pd( _value, rhs._value )}; }	// { a0 + a1, b0 + b1 }
		ND_ Self	HSub (const Self &rhs)						C_NE___	{ return Self{ _mm_hsub_pd( _value, rhs._value )}; }	// { a0 - a1, b0 - b1 }
		ND_ Self	AddSub (const Self &rhs)					C_NE___	{ return Self{ _mm_addsub_pd( _value, rhs._value )}; }	// { a0 - b0, a1 + b1 }
	  #endif
	};



	//
	// 128 bit int
	//
	#define AE_SIMD_Int128b
	struct Int128b
	{
	// types
	public:
		using Self		= Int128b;
		using Native_t	= __m128i;
		using Ptr_t		= AlignedPtr< sizeof(Native_t) >;
		using CPtr_t	= AlignedPtr< sizeof(Native_t), true >;


	// variables
	private:
		Native_t	_value;


	// methods
	public:
		Int128b ()											__NE___	: _value{ _mm_setzero_si128() } {}
		Int128b (Zero_t)									__NE___	: _value{ _mm_setzero_si128() } {}
		Int128b (UMax_t)									__NE___	 { auto z = _mm_setzero_si128();  _value = _mm_cmpeq_epi32( z, z ); }
		explicit Int128b (int v)							__NE___	: _value{ _mm_cvtsi32_si128( v )} {}
		explicit Int128b (slong v)							__NE___	: _value{ _mm_cvtsi64_si128( v )} {}
		explicit Int128b (const Native_t &v)				__NE___	: _value{ v } {}
		explicit Int128b (const CPtr_t ptr)					__NE___	: _value{ _mm_load_si128( ptr.Cast<Native_t>() )} {}

		ND_ Native_t &		Ref ()							__NE___	{ return _value; }
		ND_ Native_t const&	Ref ()							C_NE___	{ return _value; }


	// per bit operations //
		ND_ Self	operator ~ ()							C_NE___	{ return BitInverse(); }

		ND_ Self	operator & (const Self &rhs)			C_NE___	{ return And( rhs ); }
		ND_ Self	operator | (const Self &rhs)			C_NE___	{ return Or( rhs ); }
		ND_ Self	operator ^ (const Self &rhs)			C_NE___	{ return Xor( rhs ); }

		ND_ Self	BitInverse ()							C_NE___	{ return AndNot( Self{UMax} ); }

		ND_ Self	And (const Self &rhs)					C_NE___	{ return Self{ _mm_and_si128( _value, rhs._value )}; }
		ND_ Self	Or  (const Self &rhs)					C_NE___	{ return Self{ _mm_or_si128( _value, rhs._value )}; }
		ND_ Self	Xor (const Self &rhs)					C_NE___	{ return Self{ _mm_xor_si128( _value, rhs._value )}; }
		ND_ Self	AndNot (const Self &rhs)				C_NE___	{ return Self{ _mm_andnot_si128( _value, rhs._value )}; } // ~a & b

	  #if AE_SIMD_SSE >= 41
		// return 0 or 1
		ND_ int		TestAnd (const Self &mask)				C_NE___	{ return _mm_testz_si128( _value, mask._value ); }		// a & mask == 0 ? 1 : 0
		ND_ int		TestAndNot (const Self &mask)			C_NE___	{ return _mm_testc_si128( _value, mask._value ); }		// a & ~mask == 0 ? 1 : 0
		ND_ int		TestBoth_And_AndNot (const Self &mask)	C_NE___	{ return _mm_testnzc_si128( _value, mask._value ); }	// (a & mask == 0) and (a & ~mask == 0) ? 1 : 0
	  #endif

		// shift 128bit value in bytes
		template <uint ShiftBytes> ND_ Self  LByteShift ()	C_NE___;
		template <uint ShiftBytes> ND_ Self  RByteShift ()	C_NE___;


	// conversion //
		ND_ int		ToInt32 ()								C_NE___	{ return _mm_cvtsi128_si32( _value ); }
		ND_ slong	ToInt64 ()								C_NE___	{ return _mm_cvtsi128_si64( _value ); }

		template <typename T>
		ND_ auto	ToArray ()								C_NE___;
	};



	//
	// 128 bit integer (SSE 2.x)
	//
	#define AE_SIMD_SimdTInt128
	template <typename IntType>
	struct SimdTInt128
	{
		StaticAssert( IsInteger<IntType> );

	// types
	public:
		using Scalar_t		= IntType;
		using Self			= SimdTInt128< IntType >;
		using Bool_t		= SimdTInt128< ToUnsignedInteger< IntType >>;
		using Native_t		= __m128i;
		using Ptr_t			= AlignedPtr< sizeof(Native_t) >;
		using CPtr_t		= AlignedPtr< sizeof(Native_t), true >;
		using Signed_t		= SimdTInt128< ToSignedInteger< IntType >>;
		using Unsigned_t	= SimdTInt128< ToUnsignedInteger< IntType >>;
		using Shift64_t		= SimdTInt128< slong >;

		static constexpr uint	parts	= 1;
		static constexpr uint	count	= sizeof(Native_t) / sizeof(IntType);
		using Array_t					= StaticArray< Scalar_t, count >;
		StaticAssert( sizeof(Array_t) == sizeof(Native_t) );

		using Mask_t		= MSBMask< count, CT_IntLog2< 16/count >>;


	// variables
	private:
		Native_t	_value;		// int64[2], int32[4], int16[8], int8[16]

		static constexpr bool	is8		= sizeof( IntType ) == 1;
		static constexpr bool	is16	= sizeof( IntType ) == 2;
		static constexpr bool	is32	= sizeof( IntType ) == 4;
		static constexpr bool	is64	= sizeof( IntType ) == 8;

		static constexpr bool	isU8	= IsSame< IntType, ubyte >;
		static constexpr bool	isU16	= IsSame< IntType, ushort >;
		static constexpr bool	isU32	= IsSame< IntType, uint >;
		static constexpr bool	isU64	= IsSame< IntType, ulong >;

		static constexpr bool	isI8	= IsSame< IntType, sbyte >;
		static constexpr bool	isI16	= IsSame< IntType, sshort >;
		static constexpr bool	isI32	= IsSame< IntType, sint >;
		static constexpr bool	isI64	= IsSame< IntType, slong >;

		StaticAssert( is8 or is16 or is32 or is64 );
		StaticAssert( isU8 or isU16 or isU32 or isU64 or isI8 or isI16 or isI32 or isI64 );


	// methods
	public:
		SimdTInt128 ()										__NE___	: _value{ _mm_setzero_si128() } {}
		SimdTInt128 (Zero_t)								__NE___	: _value{ _mm_setzero_si128() } {}
		SimdTInt128 (UMax_t)								__NE___	 { auto z = _mm_setzero_si128();  _value = _mm_cmpeq_epi32( z, z ); }
		explicit SimdTInt128 (const Native_t &v)			__NE___	: _value{ v } {}
		explicit SimdTInt128 (const CPtr_t ptr)				__NE___	: _value{ _mm_load_si128( ptr.Cast<Native_t>() )} {}
		explicit SimdTInt128 (const Scalar_t* ptr)			__NE___	: _value{ _mm_loadu_si128( reinterpret_cast<Native_t const *>( GetNonNull( ptr )) )} {}
		explicit SimdTInt128 (Scalar_t v)					__NE___;

		template <uint Step>
		explicit SimdTInt128 (MSBMask<count, Step> mask)	__NE___;

		template <typename T = Scalar_t> requires( sizeof(T)==8 )
		explicit SimdTInt128 (const SimdDouble2::Bool2 &v)	__NE___ : _value{ _mm_castpd_si128( v.Ref() )} {}

		template <typename T = Scalar_t> requires( sizeof(T)==4 )
		explicit SimdTInt128 (const SimdFloat4::Bool4 &v)	__NE___ : _value{ _mm_castps_si128( v.Ref() )} {}

		template <typename T = Scalar_t> requires( sizeof(T)==1 )
		SimdTInt128 (Scalar_t v00, Scalar_t v01, Scalar_t v02, Scalar_t v03,
					 Scalar_t v04, Scalar_t v05, Scalar_t v06, Scalar_t v07,
					 Scalar_t v08, Scalar_t v09, Scalar_t v10, Scalar_t v11,
					 Scalar_t v12, Scalar_t v13, Scalar_t v14, Scalar_t v15) __NE___ :
			_value{ _mm_set_epi8( v15, v14, v13, v12, v11, v10, v09, v08,
								  v07, v06, v05, v04, v03, v02, v01, v00 )} {}

		template <typename T = Scalar_t> requires( sizeof(T)==2 )
		SimdTInt128 (Scalar_t v0, Scalar_t v1, Scalar_t v2, Scalar_t v3,
					 Scalar_t v4, Scalar_t v5, Scalar_t v6, Scalar_t v7) __NE___ :
			_value{ _mm_set_epi16( v7, v6, v5, v4, v3, v2, v1, v0 )} {}

		template <typename T = Scalar_t> requires( sizeof(T)==4 )
		SimdTInt128 (Scalar_t v0, Scalar_t v1, Scalar_t v2, Scalar_t v3) __NE___ :
			_value{ _mm_set_epi32( v3, v2, v1, v0 )} {}

		template <typename T = Scalar_t> requires( sizeof(T)==8 )
		SimdTInt128 (Scalar_t v0, Scalar_t v1)				__NE___ :
			_value{ _mm_set_epi64x( v1, v0 )} {}


		template <typename T = Scalar_t> requires( IsUnsignedInteger<T> )
		explicit SimdTInt128 (bool v)						__NE___ : SimdTInt128{ v ? Scalar_t{UMax} : Scalar_t{0} } {}

		template <typename B, typename T=Scalar_t> requires( IsSame< T, ubyte > and IsSame< B, bool >)
		SimdTInt128 (B v00, B v01, B v02, B v03, B v04, B v05, B v06, B v07,
					 B v08, B v09, B v10, B v11, B v12, B v13, B v14, B v15) __NE___;

		template <typename B, typename T=Scalar_t> requires( IsSame< T, ushort > and IsSame< B, bool >)
		SimdTInt128 (B v0, B v1, B v2, B v3,
					 B v4, B v5, B v6, B v7)				__NE___;

		template <typename B, typename T=Scalar_t> requires( IsSame< T, uint > and IsSame< B, bool >)
		SimdTInt128 (B v0, B v1, B v2, B v3)				__NE___;

		template <typename B, typename T=Scalar_t> requires( IsSame< T, ulong > and IsSame< B, bool >)
		SimdTInt128 (B v0, B v1)							__NE___;


		template <typename T = Scalar_t> requires( IsSignedInteger<T> )
		ND_ Self	operator - ()							C_NE___	{ return Negative(); }

		template <typename T = Scalar_t> requires( IsUnsignedInteger<T> )
		ND_ Bool_t	operator ! ()							C_NE___	{ return BitInverse(); }

		ND_ Self	operator ~ ()							C_NE___	{ return BitInverse(); }

		ND_ Self	operator + (const Self &rhs)			C_NE___	{ return Add( rhs ); }
		ND_ Self	operator - (const Self &rhs)			C_NE___	{ return Sub( rhs ); }
		ND_ Self	operator * (const Self &rhs)			C_NE___	{ return Mul( rhs ); }

		ND_ Self	operator + (const Scalar_t rhs)			C_NE___	{ return Add( rhs ); }
		ND_ Self	operator - (const Scalar_t rhs)			C_NE___	{ return Sub( rhs ); }
		ND_ Self	operator * (const Scalar_t rhs)			C_NE___	{ return Mul( rhs ); }

		ND_ Self	operator & (const Self &rhs)			C_NE___	{ return And( rhs ); }
		ND_ Self	operator | (const Self &rhs)			C_NE___	{ return Or( rhs ); }
		ND_ Self	operator ^ (const Self &rhs)			C_NE___	{ return Xor( rhs ); }

		ND_ Self	operator & (const Scalar_t rhs)			C_NE___	{ return And( rhs ); }
		ND_ Self	operator | (const Scalar_t rhs)			C_NE___	{ return Or( rhs ); }
		ND_ Self	operator ^ (const Scalar_t rhs)			C_NE___	{ return Xor( rhs ); }

		ND_ Self	operator << (uint shift)				C_NE___	{ return LShift( shift ); }
		ND_ Self	operator << (const Shift64_t &shift)	C_NE___	{ return LShift( shift ); }
		ND_ Self	operator << (const Unsigned_t &shift)	C_NE___	{ return LShift( shift ); }

		ND_ Self	operator >> (uint shift)				C_NE___	{ return RShift( shift ); }
		ND_ Self	operator >> (const Shift64_t &shift)	C_NE___	{ return RShift( shift ); }
		ND_ Self	operator >> (const Unsigned_t &shift)	C_NE___	{ return RShift( shift ); }

		ND_ Bool_t  operator == (const Self &rhs)			C_NE___	{ return Equal( rhs ); }
		ND_ Bool_t  operator != (const Self &rhs)			C_NE___	{ return NotEqual( rhs ); }
		ND_ Bool_t  operator >  (const Self &rhs)			C_NE___	{ return Greater( rhs ); }
		ND_ Bool_t  operator <  (const Self &rhs)			C_NE___	{ return Less( rhs ); }
		ND_ Bool_t  operator >= (const Self &rhs)			C_NE___	{ return GEqual( rhs ); }
		ND_ Bool_t  operator <= (const Self &rhs)			C_NE___	{ return LEqual( rhs ); }

		template <uint I> ND_ Scalar_t	get ()				C_NE___;
		template <uint I> ND_ Self		set (Scalar_t val)	C_NE___;

		ND_ Native_t &		Ref ()							__NE___	{ return _value; }
		ND_ Native_t const&	Ref ()							C_NE___	{ return _value; }


		template <typename T = Scalar_t> requires( IsSignedInteger<T> )
		ND_ Self	Negative ()								C_NE___	{ return Self{0}.Sub( *this ); }		// TODO: optimize ?

		template <typename T = Scalar_t> requires( IsSignedInteger<T> )
		ND_ Self	Abs ()									C_NE___;	// SSSE3, AVX512

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

		ND_ friend Self  Select (const Bool_t &condition, const Self &ifTrue, const Self &ifFalse) __NE___ { return _Select( condition, ifTrue, ifFalse ); }


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


		ND_ Mask_t	ToBitfield ()							C_NE___	{ return Mask_t{_mm_movemask_epi8( _value )}; }

		// for comparison functions
		template <typename T = Scalar_t> requires( IsUnsignedInteger<T> )
		ND_ bool	All ()									C_NE___	{ return _mm_movemask_epi8( _value ) == 0xFFFF; }

		template <typename T = Scalar_t> requires( IsUnsignedInteger<T> )
		ND_ bool	Any ()									C_NE___	{ return _mm_movemask_epi8( _value ) != 0; }

		template <typename T = Scalar_t> requires( IsUnsignedInteger<T> )
		ND_ bool	None ()									C_NE___	{ return _mm_movemask_epi8( _value ) == 0; }


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

		ND_ Self	LShift_LogicV (const Unsigned_t &shift)	C_NE___;
		ND_ Self	RShift_LogicV (const Unsigned_t &shift)	C_NE___;
		ND_ Self	RShift_ArithV (const Unsigned_t &shift)	C_NE___;

		template <uint Shift>	ND_ Self  LShift ()					C_NE___	{ return RShift_Arith< Shift >(); }
		template <uint Shift>	ND_ Self  RShift ()					C_NE___	{ return RShift_Arith< Shift >(); }
		template <typename T>	ND_ Self  LShift (const T &shift)	C_NE___;
		template <typename T>	ND_ Self  RShift (const T &shift)	C_NE___;


	// per bit operations //
		ND_ Self	BitInverse ()							C_NE___	{ return AndNot( Self{UMax} ); }

		ND_ Self	And (const Self &rhs)					C_NE___	{ return Self{ _mm_and_si128( _value, rhs._value )}; }
		ND_ Self	Or  (const Self &rhs)					C_NE___	{ return Self{ _mm_or_si128( _value, rhs._value )}; }
		ND_ Self	Xor (const Self &rhs)					C_NE___	{ return Self{ _mm_xor_si128( _value, rhs._value )}; }
		ND_ Self	AndNot (const Self &rhs)				C_NE___	{ return Self{ _mm_andnot_si128( _value, rhs._value )}; }	// ~a & b

		ND_ Self	And (const Scalar_t rhs)				C_NE___	{ return And( Self{rhs} ); }
		ND_ Self	Or  (const Scalar_t rhs)				C_NE___	{ return Or(  Self{rhs} ); }
		ND_ Self	Xor (const Scalar_t rhs)				C_NE___	{ return Xor( Self{rhs} ); }
		ND_ Self	AndNot (const Scalar_t rhs)				C_NE___	{ return AndNot( Self{rhs} ); }	// ~a & b


	// conversion //
		template <uint Idx>
		ND_ Self	Part ()									C_NE___ { StaticAssert( Idx < parts );  return *this; }

		template <uint V0, uint V1, uint V2, uint V3,
				  uint V4, uint V5, uint V6, uint V7,
				  uint V8, uint V9, uint V10, uint V11,
				  uint V12, uint V13, uint V14, uint V15,
				  typename T=Scalar_t> requires( sizeof(T)==1 )
		ND_ Self	Swizzle ()								C_NE___;

		template <uint V0, uint V1, uint V2, uint V3,
				  uint V4, uint V5, uint V6, uint V7,
				  typename T=Scalar_t> requires( sizeof(T)==2 )
		ND_ Self	Swizzle ()								C_NE___;

		template <uint X, uint Y, uint Z, uint W,  typename T=Scalar_t> requires( sizeof(T)==4 )
		ND_ Self	Swizzle ()								C_NE___;

		template <uint X, uint Y,  typename T=Scalar_t> requires( sizeof(T)==8 )
		ND_ Self	Swizzle ()								C_NE___;

		template <uint V0, uint V1, uint V2, uint V3,
				  uint V4, uint V5, uint V6, uint V7,
				  typename T=Scalar_t> requires( sizeof(T)==2 )
		ND_ Self	Shuffle (const Self &v8)				C_NE___;

		template <uint X, uint Y, uint Z, uint W,  typename T=Scalar_t> requires( sizeof(T)==4 )
		ND_ Self	Shuffle (const Self &v4567)				C_NE___;

		template <uint X, uint Y,  typename T=Scalar_t> requires( sizeof(T)==8 )
		ND_ Self	Shuffle (const Self &v23)				C_NE___;

		ND_ Array_t	ToArray ()								C_NE___	{ Array_t arr;  ToArray( OUT arr.data() );  return arr; }
		void		ToArray (OUT Scalar_t* dst)				C_NE___	{ NonNull( dst );  _mm_storeu_si128( OUT reinterpret_cast<Native_t *>(dst), _value ); }
		void		ToAlignedArray (OUT Ptr_t dst)			C_NE___	{ _mm_store_si128( OUT dst.Cast<Native_t>(), _value ); }

		template <typename DstType>
		ND_ DstType	BitCast ()								C_NE___;

		template <typename DstScalar>
		ND_ auto	Convert ()								C_NE___;

		template <typename T = Scalar_t> requires( IsUnsignedInteger<T> )
		ND_ auto	ToSigned ()								C_NE___;

		template <typename T = Scalar_t> requires( IsSignedInteger<T> )
		ND_ auto	ToUnsigned ()							C_NE___;

		template <uint I=0>	ND_ auto	ToHalf ()			C_NE___;
		template <uint I=0>	ND_ auto	ToFloat ()			C_NE___;
		template <uint I=0>	ND_ auto	ToDouble ()			C_NE___;
		template <uint I=0>	ND_ auto	ToDouble4 ()		C_NE___;	// AVX
		template <uint I=0>	ND_ auto	ToShort ()			C_NE___;
		template <uint I=0>	ND_ auto	ToInt ()			C_NE___;
		template <uint I=0>	ND_ auto	ToLong ()			C_NE___;

		template <typename T = Scalar_t> requires( sizeof(T)>=4 )
		ND_ explicit operator PackedVec<T,count> ()			C_NE___
		{
			StaticAssert( IsSame< T, Scalar_t >);
			StaticAssert( count <= 4 );
			PackedVec<T,count> tmp;
			_mm_storeu_si128( OUT reinterpret_cast<Native_t *>(&tmp.x), _value );
			return tmp;
		}



	// Features //
		NdCe__ static bool  Has_Arithmetic ()				{ return true; }
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
		NdCe__ static bool  Has_Swizzle ()					{ return true; }
		NdCe__ static bool  Has_Shuffle ();

		template <typename DstType>	NdCe__ static bool  Has_BitCast ();
		template <typename DstType>	NdCe__ static bool  Has_Convert ();

	private:
		friend struct SimdTInt256<Scalar_t>;

		ND_ static Self  _Select (const Bool_t &condition, const Self &ifTrue, const Self &ifFalse) __NE___;

		template <uint Idx = 0, typename T = Scalar_t> requires( IsSame<T,int> )
		ND_ SimdDouble2	_IntToDouble2 ()					C_NE___;

		template <uint Idx = 0, typename T = Scalar_t> requires( IsSame<T,uint> )
		ND_ SimdDouble2	_UIntToDouble2 ()					C_NE___;

		template <typename T = Scalar_t> requires( IsSame<T,int> )
		ND_ SimdDouble4	_IntToDouble4 ()					C_NE___;	// AVX

		template <typename T = Scalar_t> requires( IsSame<T,uint> )
		ND_ SimdDouble4	_UIntToDouble4 ()					C_NE___;	// AVX

	  #if AE_SIMD_AVX >= 31  // AVX512DQ, AVX512VL
		template <typename T = Scalar_t> requires( IsSame<T,slong> )
		ND_ SimdDouble2	_LongToDouble2 ()					C_NE___;

		template <typename T = Scalar_t> requires( IsSame<T,ulong> )
		ND_ SimdDouble2	_ULongToDouble2 ()					C_NE___;
	  #endif
	};



	//
	// 128 bit half (F16C, AVX512_FP16, AVX512VL)
	//
	#define AE_SIMD_SimdHalf8
	struct SimdHalf8
	{
	// types
	public:
		static constexpr uint	count	= 8;
		static constexpr uint	parts	= 1;

		using Scalar_t		= half;
		using Self			= SimdHalf8;
		using Native_t		= __m128i;	// __m128h	- MSVC & Clang use __m128i, GCC use custom type
		using Array_t		= StaticArray< Scalar_t, count >;
		using Ptr_t			= AlignedPtr< sizeof(Native_t) >;
		using CPtr_t		= AlignedPtr< sizeof(Native_t), true >;
		using SimdInt_t		= SimdShort8;
		using SimdUInt_t	= SimdUShort8;
		using Mask_t		= MSBMask< count, 1 >;

		StaticAssert( sizeof(Array_t) == sizeof(Native_t) );

		struct Bool8
		{
		private:
			__m128i		_value;		// ushort[8]

		public:
			explicit Bool8 (bool val)						__NE___	: _value{ val ? True()._value : False()._value } {}
			explicit Bool8 (const __m128i &val)				__NE___	: _value{ val } {}
			explicit Bool8 (const SimdInt_t &v)				__NE___	: _value{ v.Ref() } {}
			explicit Bool8 (const SimdUInt_t &v)			__NE___	: _value{ v.Ref() } {}

			Bool8 (bool v0, bool v1, bool v2, bool v3,
				   bool v4, bool v5, bool v6, bool v7)		__NE___ : Bool8{ SimdUInt_t{ v0, v1, v2, v3, v4, v5, v6, v7 }} {}

			ND_ Bool8	operator | (const Bool8 &rhs)		C_NE___	{ return Bool8{ _mm_or_si128(  _value, rhs._value )}; }
			ND_ Bool8	operator & (const Bool8 &rhs)		C_NE___	{ return Bool8{ _mm_and_si128( _value, rhs._value )}; }
			ND_ Bool8	operator ^ (const Bool8 &rhs)		C_NE___	{ return Bool8{ _mm_xor_si128( _value, rhs._value )}; }
			ND_ Bool8	operator ! ()						C_NE___	{ return Bool8{ _mm_andnot_si128( _value, True()._value )}; }

			ND_ Bool8	operator == (const Bool8 &rhs)		C_NE___	{ return Bool8{ _mm_and_si128( _value, rhs._value )}; }
			ND_ Bool8	operator != (const Bool8 &rhs)		C_NE___	{ return Bool8{ _mm_andnot_si128( _value, rhs._value )}; }

			ND_ Bool8	AndNot (const Bool8 &rhs)			C_NE___	{ return Bool8{ _mm_andnot_si128( _value, rhs._value )}; }		// ~a & b
			ND_ Self	AndNot (const Self &rhs)			C_NE___	{ return Self{ _mm_andnot_si128( _value, rhs.Ref() )}; }		// ~a & b

			ND_ bool	All ()								C_NE___	{ return _mm_movemask_epi8( _value ) == 0xFFFF; }
			ND_ bool	Any ()								C_NE___	{ return _mm_movemask_epi8( _value ) != 0; }
			ND_ bool	None ()								C_NE___	{ return _mm_movemask_epi8( _value ) == 0; }

			ND_ auto&	Ref ()								C_NE___	{ return _value; }

			ND_ Mask_t	ToBitfield ()						C_NE___	{ return Mask_t{_mm_movemask_epi8( _value )}; }

			template <typename DstType>
			ND_ DstType	BitCast ()							C_NE___;

			ND_ explicit operator SimdInt_t ()				C_NE___	{ return SimdInt_t{ _value }; }
			ND_ explicit operator SimdUInt_t ()				C_NE___	{ return SimdUInt_t{ _value }; }

			ND_ static Bool8	True ()						__NE___	{ return Bool8{ SimdUInt_t{UMax} }; }
			ND_ static Bool8	False ()					__NE___	{ return Bool8{ _mm_setzero_si128() }; }
		};
		using Bool_t = Bool8;


	// variables
	private:
		Native_t	_value;		// half[8]

		static constexpr ushort		c_One_BitMask = 0x3c00;	// bitcast<ushort>( 1.hf )


	// methods
	public:
		SimdHalf8 ()											__NE___	: _value{ _mm_setzero_si128() } {}
		SimdHalf8 (Zero_t)										__NE___	: _value{ _mm_setzero_si128() } {}
		explicit SimdHalf8 (Scalar_t v)							__NE___	: _value{ _mm_set1_epi16( Base::BitCast<short>( v ))} {}
		explicit SimdHalf8 (float v)							__NE___ : SimdHalf8{ Scalar_t{v} } {}
		explicit SimdHalf8 (const Native_t &v)					__NE___	: _value{ v } {}
		explicit SimdHalf8 (const Scalar_t* ptr)				__NE___	: _value{ _mm_loadu_si128( reinterpret_cast<__m128i const *>( GetNonNull( ptr )) )} {}
		explicit SimdHalf8 (const CPtr_t ptr)					__NE___	: _value{ _mm_load_si128( ptr.Cast<__m128i>() )} {}
		explicit SimdHalf8 (const Bool8 &v)						__NE___ : _value{ _mm_and_si128( v.Ref(), _mm_set1_epi16(c_One_BitMask) )} {}

		explicit SimdHalf8 (const SimdFloat4 &v0123)			__NE___;
		SimdHalf8 (const SimdFloat4 &v0123, const SimdFloat4 &v4567)	__NE___;

		SimdHalf8 (Scalar_t v0, Scalar_t v1, Scalar_t v2, Scalar_t v3,
				   Scalar_t v4, Scalar_t v5, Scalar_t v6, Scalar_t v7)	__NE___;
		SimdHalf8 (float v0, float v1, float v2, float v3,
				   float v4, float v5, float v6, float v7)				__NE___;

		ND_ Bool8	operator == (const Self &rhs)				C_NE___	{ return Equal( rhs ); }
		ND_ Bool8	operator != (const Self &rhs)				C_NE___	{ return NotEqual( rhs ); }

		template <uint I> ND_ Scalar_t	get ()					C_NE___;
		template <uint I> ND_ Self		set (Scalar_t val)		C_NE___;

		ND_ Native_t &		Ref ()								__NE___	{ return _value; }
		ND_ Native_t const&	Ref ()								C_NE___	{ return _value; }

	  #ifndef AE_COMPILER_GCC
		ND_ Self	Abs ()										C_NE___	{ return Self{ _mm_and_epi32( _mm_set1_epi32(0x7FFF7FFF), _value )}; }
	  #endif

		ND_ Bool8	Equal    (const Self &rhs)					C_NE___	{ return Bool8{ _mm_cmpeq_epi16( _value, rhs._value )}; }
		ND_ Bool8	NotEqual (const Self &rhs)					C_NE___	{ return not Equal( rhs ); }

		// compare and return 0.0 or 1.0
		ND_ Self	EqualF    (const Self &rhs)					C_NE___	{ return Self{ Equal( rhs )}; }
		ND_ Self	NotEqualF (const Self &rhs)					C_NE___	{ return Self{ NotEqual( rhs )}; }

		ND_ Bool8	BitEqual (const Self&, EnabledBitCount acc)	C_NE___;	// TODO

		friend Self  Select (const Bool_t &condition, const Self &ifTrue, const Self &ifFalse) __NE___;


	// per bit operations //
		ND_ Self	BitInverse ()								C_NE___	{ return AndNot( Bool8::True() ); }

		ND_ Self	And (const Self &rhs)						C_NE___	{ return Self{ _mm_and_si128( _value, rhs._value )}; }
		ND_ Self	Or  (const Self &rhs)						C_NE___	{ return Self{ _mm_or_si128( _value, rhs._value )}; }
		ND_ Self	Xor (const Self &rhs)						C_NE___	{ return Self{ _mm_xor_si128( _value, rhs._value )}; }
		ND_ Self	AndNot (const Self &rhs)					C_NE___	{ return Self{ _mm_andnot_si128( _value, rhs._value )}; } // ~a & b

		ND_ Self	And (const Bool8 &rhs)						C_NE___	{ return Self{ _mm_and_si128( _value, rhs.Ref() )}; }
		ND_ Self	Or  (const Bool8 &rhs)						C_NE___	{ return Self{ _mm_or_si128( _value, rhs.Ref() )}; }
		ND_ Self	Xor (const Bool8 &rhs)						C_NE___	{ return Self{ _mm_xor_si128( _value, rhs.Ref() )}; }
		ND_ Self	AndNot (const Bool8 &rhs)					C_NE___	{ return Self{ _mm_andnot_si128( _value, rhs.Ref() )}; } // ~a & b

		ND_ Self	And (const ushort rhs)						C_NE___	{ return Self{ _mm_and_si128( _value, _mm_set1_epi16(rhs) )}; }
		ND_ Self	Or  (const ushort rhs)						C_NE___	{ return Self{ _mm_or_si128( _value, _mm_set1_epi16(rhs) )}; }
		ND_ Self	Xor (const ushort rhs)						C_NE___	{ return Self{ _mm_xor_si128( _value, _mm_set1_epi16(rhs) )}; }
		ND_ Self	AndNot (const ushort rhs)					C_NE___	{ return Self{ _mm_andnot_si128( _value, _mm_set1_epi16(rhs) )}; } // ~a & b


	// conversion //
		template <typename DstType>
		ND_ DstType	BitCast ()									C_NE___;

		template <uint V0, uint V1, uint V2, uint V3,
				  uint V4, uint V5, uint V6, uint V7>
		ND_ Self	Swizzle ()									C_NE___;

		template <uint Idx>
		ND_ Self	Part ()										C_NE___	{ StaticAssert( Idx < parts );  return *this; }

		template <typename DstScalar>
		ND_ auto	Convert ()									C_NE___;

		ND_ Array_t	ToArray ()									C_NE___	{ Array_t arr;  ToArray( OUT arr.data() );  return arr; }
		void		ToArray (OUT Scalar_t* dst)					C_NE___	{ NonNull( dst );  _mm_storeu_si128( OUT reinterpret_cast<__m128i *>(dst), _value ); }
		void		ToAlignedArray (OUT Ptr_t dst)				C_NE___	{ _mm_store_si128( OUT dst.Cast<__m128i>(), _value ); }

		template <uint I>
		ND_ auto	ToFloat ()									C_NE___;

		ND_ SimdFloat8	ToFloat8 ()								C_NE___;


	  #if 0 //AE_SIMD_AVX512_FP16

		// TODO

	  #endif // AE_SIMD_AVX512_FP16


	// Features //
		NdCe__ static bool  Has_Arithmetic ()					{ return false; }
		NdCe__ static bool  Has_Equal ()						{ return true; }
		NdCe__ static bool  Has_Greater ()						{ return false; }
		NdCe__ static bool  Has_Rounding ()						{ return false; }
		NdCe__ static bool  Has_FusedMulAdd ()					{ return false; }
		NdCe__ static bool  Has_MulAdd ()						{ return false; }
		NdCe__ static bool  Has_PreciseSqrt ()					{ return false; }
		NdCe__ static bool  Has_PreciseInvSqrt ()				{ return false; }
		NdCe__ static bool  Has_PreciseDiv ()					{ return false; }
		NdCe__ static bool  Has_ApproxReciprocal ()				{ return false; }
		NdCe__ static bool  Has_ApproxInvSqrt ()				{ return false; }
		NdCe__ static bool  Has_Trigonometry ()					{ return false; }
		NdCe__ static bool  Has_Exponential ()					{ return false; }
		NdCe__ static bool  Has_ReduceAdd ()					{ return false; }
		NdCe__ static bool  Has_ReduceMinMax ()					{ return false; }
		NdCe__ static bool  Has_InclusiveAdd ()					{ return false; }
		NdCe__ static bool  Has_BitEqual ()						{ return false; }
		NdCe__ static bool  Has_Swizzle ()						{ return true; }
		NdCe__ static bool  Has_Shuffle ()						{ return false; }

		template <typename DstType>	NdCe__ static bool  Has_BitCast ();
		template <typename DstType>	NdCe__ static bool  Has_Convert ();
	};

//-----------------------------------------------------------------------------

} // AE::Base
#endif // AE_SIMD_SSE
