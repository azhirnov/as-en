// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

namespace AE::Base
{

#ifdef AE_SIMD_SimdHalf4
/*
=================================================
	Bool4: ctor
=================================================
*/
	inline SimdHalf4::Bool4::Bool4 (bool val)				__NE___ : Bool4{SimdUShort4{ val }} {}
	inline SimdHalf4::Bool4::Bool4 (const SimdInt_t &v)		__NE___ : Bool4{ v.BitCast<SimdUInt_t>() } {}
	inline SimdHalf4::Bool4::Bool4 (const SimdUInt_t &v)	__NE___ : _value{v.Ref()} {}

	inline SimdHalf4::Bool4::Bool4 (bool v0, bool v1, bool v2, bool v3) __NE___ : Bool4{ SimdUInt_t{ v0, v1, v2, v3 }} {}

/*
=================================================
	Bool4: All / Any / None
=================================================
*/
	inline bool  SimdHalf4::Bool4::All ()		C_NE___	{ return SimdUInt_t{ _value }.All(); }
	inline bool  SimdHalf4::Bool4::Any ()		C_NE___	{ return SimdUInt_t{ _value }.Any(); }
	inline bool  SimdHalf4::Bool4::None ()		C_NE___	{ return SimdUInt_t{ _value }.None(); }

	inline SimdHalf4::Mask_t  SimdHalf4::Bool4::ToBitfield () C_NE___ { return SimdUInt_t{ _value }.ToBitfield(); }

/*
=================================================
	Bool4: BitCast
=================================================
*/
	template <typename DstType>
	DstType  SimdHalf4::Bool4::BitCast () C_NE___
	{
		if constexpr( IsSame< DstType, SimdUShort4 >)
			return SimdUShort4{ _value };
		else
			return SimdUShort4{ _value }.BitCast< DstType >();
	}

/*
=================================================
	constructor
=================================================
*/
	inline SimdHalf4::SimdHalf4 (Scalar_t x, Scalar_t y, Scalar_t z, Scalar_t w) __NE___
	{
		Scalar_t a[]={x,y,z,w};
		_value = vld1_f16( Base::Cast<float16_t>( a ));
	}

	inline SimdHalf4::SimdHalf4 (float16_t x, float16_t y, float16_t z, float16_t w) __NE___
	{
		float16_t a[]={x,y,z,w};
		_value = vld1_f16( a );
	}

/*
=================================================
	Convert
----
	same as C++ style conversion 'Dst(src)'
=================================================
*/
	template <typename DstScalar>
	auto  SimdHalf4::Convert ()  C_NE___
	{
		StaticAssert( IsAnyScalar< DstScalar >);
		StaticAssert( not IsSame< DstScalar, Scalar_t >);
		StaticAssert( Has_Convert< DstScalar >() );

	  #ifdef AE_SIMD_SimdFloat4
		if constexpr( IsSame< DstScalar, float >)
			return SimdFloat4{ vcvt_f32_f16( _value )};
	  #endif

		if constexpr( IsSame< DstScalar, sshort >)
			return SimdShort4{ vcvt_s16_f16( _value )};

		if constexpr( IsSame< DstScalar, ushort >)
			return SimdUShort4{ vcvt_u16_f16( _value )};
	}

/*
=================================================
	Has_Convert
=================================================
*/
	template <typename DstScalar>
	__Ce__ bool  SimdHalf4::Has_Convert ()
	{
		if constexpr( IsSame< DstScalar, Scalar_t >)
			return false;
		else
	  #ifdef AE_SIMD_SimdFloat4
		if constexpr( IsSame< DstScalar, float >)
			return true;
		else
	  #endif
		if constexpr( IsSame< DstScalar, sshort > or IsSame< DstScalar, ushort >)
			return true;
		else
			return false;
	}

/*
=================================================
	BitCast
=================================================
*/
	template <typename DstType>
	DstType  SimdHalf4::BitCast ()  C_NE___
	{
		StaticAssert( sizeof(Self) == sizeof(DstType) );
		StaticAssert( Has_BitCast<DstType>() );

		if constexpr( IsSame< DstType, SimdInt_t > or IsSame< DstType, SimdUInt_t >)
			return DstType{ vreinterpret_u16_f16( _value )};
	}

/*
=================================================
	Has_BitCast
=================================================
*/
	template <typename DstType>
	__Ce__ bool  SimdHalf4::Has_BitCast ()
	{
		if constexpr( IsSame< DstType, SimdInt_t > or IsSame< DstType, SimdUInt_t >)
			return true;
		else
			return false;
	}

/*
=================================================
	Swizzle
=================================================
*/
	template <uint X, uint Y, uint Z, uint W>
	SimdHalf4  SimdHalf4::Swizzle ()  C_NE___
	{
		StaticAssert( Has_Swizzle() );
		StaticAssert( X < count );
		StaticAssert( Y < count );
		StaticAssert( Z < count );
		StaticAssert( W < count );
		return Self{ __builtin_shufflevector( _value, _value, X, Y, Z, W )};
	}

#endif // AE_SIMD_SimdHalf4
//-----------------------------------------------------------------------------



#ifdef AE_SIMD_SimdTInt64
/*
=================================================
	constructor
=================================================
*/
	template <typename IT>
	SimdTInt64<IT>::SimdTInt64 (IT val)  __NE___
	{
		if constexpr( isU8 )	_value = vdup_n_u8(  val );
		if constexpr( isI8 )	_value = vdup_n_s8(  val );
		if constexpr( isU16 )	_value = vdup_n_u16( val );
		if constexpr( isI16 )	_value = vdup_n_s16( val );
		if constexpr( isU32 )	_value = vdup_n_u32( val );
		if constexpr( isI32 )	_value = vdup_n_s32( val );
	}

	template <typename IT>
	SimdTInt64<IT>::SimdTInt64 (const IT* ptr)  __NE___
	{
		NonNull( ptr );
		if constexpr( isU8 )	_value = vld1_u8(  ptr );
		if constexpr( isI8 )	_value = vld1_s8(  ptr );
		if constexpr( isU16 )	_value = vld1_u16( ptr );
		if constexpr( isI16 )	_value = vld1_s16( ptr );
		if constexpr( isU32 )	_value = vld1_u32( ptr );
		if constexpr( isI32 )	_value = vld1_s32( ptr );
	}

	template <typename IT>
	template <typename T> requires( sizeof(T)==1 )
	SimdTInt64<IT>::SimdTInt64 (Scalar_t v0, Scalar_t v1, Scalar_t v2, Scalar_t v3,
								Scalar_t v4, Scalar_t v5, Scalar_t v6, Scalar_t v7) __NE___
	{
		Scalar_t a[] = {v0, v1, v2, v3, v4, v5, v6, v7};
		StaticAssert( CountOf(a) == 8 );
		if constexpr( IsSigned<IT> )
			_value = vld1_s8( a );
		else
			_value = vld1_u8( a );
	}

	template <typename IT>
	template <typename T> requires( sizeof(T)==2 )
	SimdTInt64<IT>::SimdTInt64 (Scalar_t v0, Scalar_t v1, Scalar_t v2, Scalar_t v3) __NE___
	{
		Scalar_t a[] = {v0, v1, v2, v3};
		StaticAssert( CountOf(a) == 4 );
		if constexpr( IsSigned<IT> )
			_value = vld1_s16( a );
		else
			_value = vld1_u16( a );
	}

	template <typename IT>
	template <typename T> requires( sizeof(T)==4 )
	SimdTInt64<IT>::SimdTInt64 (Scalar_t v0, Scalar_t v1) __NE___
	{
		Scalar_t a[] = {v0, v1};
		StaticAssert( CountOf(a) == 2 );
		if constexpr( IsSigned<IT> )
			_value = vld1_s32( a );
		else
			_value = vld1_u32( a );
	}

	template <typename IT>
	template <typename B, typename T> requires( IsSame< T, ubyte > and IsSame< B, bool >)
	SimdTInt64<IT>::SimdTInt64 (B v0, B v1, B v2, B v3, B v4, B v5, B v6, B v7) __NE___
	{
		ubyte  a[] = {	T(v0 ? 0xFF : 0), T(v1 ? 0xFF : 0), T(v2 ? 0xFF : 0), T(v3 ? 0xFF : 0),
						T(v4 ? 0xFF : 0), T(v5 ? 0xFF : 0), T(v6 ? 0xFF : 0), T(v7 ? 0xFF : 0) };
		StaticAssert( CountOf(a) == 8 );
		_value = vld1_u8( a );
	}

	template <typename IT>
	template <typename B, typename T> requires( IsSame< T, ushort > and IsSame< B, bool >)
	SimdTInt64<IT>::SimdTInt64 (B v0, B v1, B v2, B v3) __NE___
	{
		ushort  a[] = { T(v0 ? 0xFFFF : 0), T(v1 ? 0xFFFF : 0), T(v2 ? 0xFFFF : 0), T(v3 ? 0xFFFF : 0) };
		StaticAssert( CountOf(a) == 4 );
		_value = vld1_u16( a );
	}

	template <typename IT>
	template <typename B, typename T> requires( IsSame< T, uint > and IsSame< B, bool >)
	SimdTInt64<IT>::SimdTInt64 (B v0, B v1) __NE___
	{
		uint  a[] = { (v0 ? ~0u : 0), (v1 ? ~0u : 0) };
		StaticAssert( CountOf(a) == 2 );
		_value = vld1_u32( a );
	}

	template <typename IT>
	template <uint Step>
	SimdTInt64<IT>::SimdTInt64 (MSBMask<count, Step> mask) __NE___
	{
		if constexpr( sizeof(IT) == 1 )
		{
			#define M(x)	sbyte(mask.template get<x>() ? -1 : 0)
			sbyte a[] = {M(0), M(1), M(2), M(3), M(4), M(5), M(6), M(7)};
			_value = vld1_s8( a );
			#undef M
		}else
		if constexpr( sizeof(IT) == 2 )
		{
			#define M(x)	short(mask.template get<x>() ? -1 : 0)
			short a[] = {M(0), M(1), M(2), M(3)};
			_value = vld1_s16( a );
			#undef M
		}else
		if constexpr( sizeof(IT) == 4 )
		{
			#define M(x)	int(mask.template get<x>() ? -1 : 0)
			int a[] = {M(0), M(1)};
			_value = vld1_s32( a );
			#undef M
		}
	}

/*
=================================================
	get / set
=================================================
*/
	template <typename IT>
	template <uint I>
	IT  SimdTInt64<IT>::get ()  C_NE___
	{
		StaticAssert( I < count );
		if constexpr( isU8 )	return vget_lane_u8(  _value, I );
		if constexpr( isI8 )	return vget_lane_s8(  _value, I );
		if constexpr( isU16 )	return vget_lane_u16( _value, I );
		if constexpr( isI16 )	return vget_lane_s16( _value, I );
		if constexpr( isU32 )	return vget_lane_u32( _value, I );
		if constexpr( isI32 )	return vget_lane_s32( _value, I );
	}

	template <typename IT>
	template <uint I>
	SimdTInt64<IT>  SimdTInt64<IT>::set (Scalar_t newValue)  C_NE___
	{
		StaticAssert( I < count );
		if constexpr( isU8 )	return Self{ vset_lane_u8(  newValue, _value, I )};
		if constexpr( isI8 )	return Self{ vset_lane_s8(  newValue, _value, I )};
		if constexpr( isU16 )	return Self{ vset_lane_u16( newValue, _value, I )};
		if constexpr( isI16 )	return Self{ vset_lane_s16( newValue, _value, I )};
		if constexpr( isU32 )	return Self{ vset_lane_u32( newValue, _value, I )};
		if constexpr( isI32 )	return Self{ vset_lane_s32( newValue, _value, I )};
	}

/*
=================================================
	Abs
=================================================
*/
	template <typename IT>
	template <typename T> requires( IsSignedInteger<T> )
	SimdTInt64<IT>  SimdTInt64<IT>::Abs ()  C_NE___
	{
		if constexpr( isI8 )	return Self{ vabs_s8( _value )};
		if constexpr( isI16 )	return Self{ vabs_s16( _value )};
		if constexpr( isI32 )	return Self{ vabs_s32( _value )};
	}

/*
=================================================
	Negative
=================================================
*/
	template <typename IT>
	template <typename T> requires( IsSignedInteger<T> )
	SimdTInt64<IT>  SimdTInt64<IT>::Negative ()  C_NE___
	{
		if constexpr( isI8 )	return Self{ vneg_s8(  _value )};
		if constexpr( isI16 )	return Self{ vneg_s16( _value )};
		if constexpr( isI32 )	return Self{ vneg_s32( _value )};
	}

/*
=================================================
	NegativeSat
=================================================
*/
	template <typename IT>
	template <typename T> requires( IsSignedInteger<T> )
	SimdTInt64<IT>  SimdTInt64<IT>::NegativeSat ()  C_NE___
	{
		if constexpr( isI8 )	return Self{ vqneg_s8(  _value )};
		if constexpr( isI16 )	return Self{ vqneg_s16( _value )};
		if constexpr( isI32 )	return Self{ vqneg_s32( _value )};
	}

/*
=================================================
	Add
=================================================
*/
	template <typename IT>
	SimdTInt64<IT>  SimdTInt64<IT>::Add (const Self &rhs)  C_NE___
	{
		if constexpr( isU8 )	return Self{ vadd_u8(  _value, rhs._value )};
		if constexpr( isI8 )	return Self{ vadd_s8(  _value, rhs._value )};
		if constexpr( isU16 )	return Self{ vadd_u16( _value, rhs._value )};
		if constexpr( isI16 )	return Self{ vadd_s16( _value, rhs._value )};
		if constexpr( isU32 )	return Self{ vadd_u32( _value, rhs._value )};
		if constexpr( isI32 )	return Self{ vadd_s32( _value, rhs._value )};
	}

/*
=================================================
	Sub
=================================================
*/
	template <typename IT>
	SimdTInt64<IT>  SimdTInt64<IT>::Sub (const Self &rhs)  C_NE___
	{
		if constexpr( isU8 )	return Self{ vsub_u8(  _value, rhs._value )};
		if constexpr( isI8 )	return Self{ vsub_s8(  _value, rhs._value )};
		if constexpr( isU16 )	return Self{ vsub_u16( _value, rhs._value )};
		if constexpr( isI16 )	return Self{ vsub_s16( _value, rhs._value )};
		if constexpr( isU32 )	return Self{ vsub_u32( _value, rhs._value )};
		if constexpr( isI32 )	return Self{ vsub_s32( _value, rhs._value )};
	}

/*
=================================================
	Mul
=================================================
*/
	template <typename IT>
	SimdTInt64<IT>  SimdTInt64<IT>::Mul (const Self &rhs)  C_NE___
	{
		StaticAssert( Has_Mul() );
		if constexpr( isU8 )	return Self{ vmul_u8(  _value, rhs._value )};
		if constexpr( isI8 )	return Self{ vmul_s8(  _value, rhs._value )};
		if constexpr( isU16 )	return Self{ vmul_u16( _value, rhs._value )};
		if constexpr( isI16 )	return Self{ vmul_s16( _value, rhs._value )};
		if constexpr( isU32 )	return Self{ vmul_u32( _value, rhs._value )};
		if constexpr( isI32 )	return Self{ vmul_s32( _value, rhs._value )};
	}

	template <typename IT>
	SimdTInt64<IT>  SimdTInt64<IT>::Mul (Scalar_t rhs)  C_NE___
	{
		StaticAssert( Has_Mul() );
		if constexpr( isU8 )	return Self{ vmul_u8( _value, vdup_n_u8(rhs) )};
		if constexpr( isI8 )	return Self{ vmul_s8( _value, vdup_n_s8(rhs) )};
		if constexpr( isU16 )	return Self{ vmul_n_u16( _value, rhs )};
		if constexpr( isI16 )	return Self{ vmul_n_s16( _value, rhs )};
		if constexpr( isU32 )	return Self{ vmul_n_u32( _value, rhs )};
		if constexpr( isI32 )	return Self{ vmul_n_s32( _value, rhs )};
	}

	template <typename IT>
	__Ce__ bool  SimdTInt64<IT>::Has_Mul ()
	{
		return sizeof(Scalar_t) <= sizeof(uint);
	}

/*
=================================================
	AddSat
=================================================
*/
	template <typename IT>
	SimdTInt64<IT>  SimdTInt64<IT>::AddSat (const Self &rhs) C_NE___
	{
		if constexpr( isU8 )	return Self{ vqadd_u8(  _value, rhs._value )};
		if constexpr( isI8 )	return Self{ vqadd_s8(  _value, rhs._value )};
		if constexpr( isU16 )	return Self{ vqadd_u16( _value, rhs._value )};
		if constexpr( isI16 )	return Self{ vqadd_s16( _value, rhs._value )};
		if constexpr( isU32 )	return Self{ vqadd_u32( _value, rhs._value )};
		if constexpr( isI32 )	return Self{ vqadd_s32( _value, rhs._value )};
	}

/*
=================================================
	AddSat
=================================================
*/
# if AE_SIMD_NEON64
	template <typename IT>
	SimdTInt64<IT>  SimdTInt64<IT>::AddSat (const _InvSign_t &rhs) C_NE___
	{
		if constexpr( isU8 )	return Self{ vsqadd_u8(  _value, rhs.Ref() )};
		if constexpr( isI8 )	return Self{ vuqadd_s8(  _value, rhs.Ref() )};
		if constexpr( isU16 )	return Self{ vsqadd_u16( _value, rhs.Ref() )};
		if constexpr( isI16 )	return Self{ vuqadd_s16( _value, rhs.Ref() )};
		if constexpr( isU32 )	return Self{ vsqadd_u32( _value, rhs.Ref() )};
		if constexpr( isI32 )	return Self{ vuqadd_s32( _value, rhs.Ref() )};
	}
# endif
/*
=================================================
	SubSat
=================================================
*/
	template <typename IT>
	SimdTInt64<IT>  SimdTInt64<IT>::SubSat (const Self &rhs) C_NE___
	{
		if constexpr( isU8 )	return Self{ vqsub_u8(  _value, rhs._value )};
		if constexpr( isI8 )	return Self{ vqsub_s8(  _value, rhs._value )};
		if constexpr( isU16 )	return Self{ vqsub_u16( _value, rhs._value )};
		if constexpr( isI16 )	return Self{ vqsub_s16( _value, rhs._value )};
		if constexpr( isU32 )	return Self{ vqsub_u32( _value, rhs._value )};
		if constexpr( isI32 )	return Self{ vqsub_s32( _value, rhs._value )};
	}

/*
=================================================
	BitInverse
=================================================
*/
	template <typename IT>
	SimdTInt64<IT>  SimdTInt64<IT>::BitInverse ()  C_NE___
	{
		if constexpr( isU8 )	return Self{ vmvn_u8(  _value )};
		if constexpr( isI8 )	return Self{ vmvn_s8(  _value )};
		if constexpr( isU16 )	return Self{ vmvn_u16( _value )};
		if constexpr( isI16 )	return Self{ vmvn_s16( _value )};
		if constexpr( isU32 )	return Self{ vmvn_u32( _value )};
		if constexpr( isI32 )	return Self{ vmvn_s32( _value )};
	}

/*
=================================================
	And
=================================================
*/
	template <typename IT>
	SimdTInt64<IT>  SimdTInt64<IT>::And (const Self &rhs)  C_NE___
	{
		if constexpr( isU8 )	return Self{ vand_u8(  _value, rhs._value )};
		if constexpr( isI8 )	return Self{ vand_s8(  _value, rhs._value )};
		if constexpr( isU16 )	return Self{ vand_u16( _value, rhs._value )};
		if constexpr( isI16 )	return Self{ vand_s16( _value, rhs._value )};
		if constexpr( isU32 )	return Self{ vand_u32( _value, rhs._value )};
		if constexpr( isI32 )	return Self{ vand_s32( _value, rhs._value )};
	}

/*
=================================================
	Or
=================================================
*/
	template <typename IT>
	SimdTInt64<IT>  SimdTInt64<IT>::Or (const Self &rhs)  C_NE___
	{
		if constexpr( isU8 )	return Self{ vorr_u8(  _value, rhs._value )};
		if constexpr( isI8 )	return Self{ vorr_s8(  _value, rhs._value )};
		if constexpr( isU16 )	return Self{ vorr_u16( _value, rhs._value )};
		if constexpr( isI16 )	return Self{ vorr_s16( _value, rhs._value )};
		if constexpr( isU32 )	return Self{ vorr_u32( _value, rhs._value )};
		if constexpr( isI32 )	return Self{ vorr_s32( _value, rhs._value )};
	}

/*
=================================================
	Xor
=================================================
*/
	template <typename IT>
	SimdTInt64<IT>  SimdTInt64<IT>::Xor (const Self &rhs)  C_NE___
	{
		if constexpr( isU8 )	return Self{ veor_u8(  _value, rhs._value )};
		if constexpr( isI8 )	return Self{ veor_s8(  _value, rhs._value )};
		if constexpr( isU16 )	return Self{ veor_u16( _value, rhs._value )};
		if constexpr( isI16 )	return Self{ veor_s16( _value, rhs._value )};
		if constexpr( isU32 )	return Self{ veor_u32( _value, rhs._value )};
		if constexpr( isI32 )	return Self{ veor_s32( _value, rhs._value )};
	}

/*
=================================================
	OrNot
=================================================
*/
	template <typename IT>
	SimdTInt64<IT>  SimdTInt64<IT>::OrNot (const Self &rhs)  C_NE___
	{
		if constexpr( isU8 )	return Self{ vorn_u8(  _value, rhs._value )};
		if constexpr( isI8 )	return Self{ vorn_s8(  _value, rhs._value )};
		if constexpr( isU16 )	return Self{ vorn_u16( _value, rhs._value )};
		if constexpr( isI16 )	return Self{ vorn_s16( _value, rhs._value )};
		if constexpr( isU32 )	return Self{ vorn_u32( _value, rhs._value )};
		if constexpr( isI32 )	return Self{ vorn_s32( _value, rhs._value )};
	}

/*
=================================================
	AndNot
=================================================
*/
	template <typename IT>
	SimdTInt64<IT>  SimdTInt64<IT>::AndNot (const Self &rhs)  C_NE___
	{
		if constexpr( isU8 )	return Self{ vbic_u8(  rhs._value, _value )};
		if constexpr( isI8 )	return Self{ vbic_s8(  rhs._value, _value )};
		if constexpr( isU16 )	return Self{ vbic_u16( rhs._value, _value )};
		if constexpr( isI16 )	return Self{ vbic_s16( rhs._value, _value )};
		if constexpr( isU32 )	return Self{ vbic_u32( rhs._value, _value )};
		if constexpr( isI32 )	return Self{ vbic_s32( rhs._value, _value )};
	}

/*
=================================================
	AllBits
=================================================
*/
	template <typename IT>
	SimdTInt64<IT>  SimdTInt64<IT>::AllBits (const Self &mask) C_NE___
	{
		if constexpr( isU8 )	return Self{ vtst_u8(  _value, mask._value )};
		if constexpr( isI8 )	return Self{ vtst_s8(  _value, mask._value )};
		if constexpr( isU16 )	return Self{ vtst_u16( _value, mask._value )};
		if constexpr( isI16 )	return Self{ vtst_s16( _value, mask._value )};
		if constexpr( isU32 )	return Self{ vtst_u32( _value, mask._value )};
		if constexpr( isI32 )	return Self{ vtst_s32( _value, mask._value )};
	}

#if AE_SIMD_NEON64
/*
=================================================
	ReduceAddScalar
=================================================
*/
	template <typename IT>
	IT  SimdTInt64<IT>::ReduceAddScalar () C_NE___
	{
		StaticAssert( Has_ReduceAdd() );
		if constexpr( isU8 )	return vaddv_u8(  _value );
		if constexpr( isI8 )	return vaddv_s8(  _value );
		if constexpr( isU16 )	return vaddv_u16( _value );
		if constexpr( isI16 )	return vaddv_s16( _value );
		if constexpr( isU32 )	return vaddv_u32( _value );
		if constexpr( isI32 )	return vaddv_s32( _value );
	}

/*
=================================================
	ReduceAddExtScalar
=================================================
*/
	template <typename IT>
	auto  SimdTInt64<IT>::ReduceAddExtScalar () C_NE___
	{
		StaticAssert( Has_ReduceAddExt() );
		if constexpr( isU8 )	return ushort{vaddlv_u8(  _value )};
		if constexpr( isI8 )	return sshort{vaddlv_s8(  _value )};
		if constexpr( isU16 )	return uint{  vaddlv_u16( _value )};
		if constexpr( isI16 )	return sint{  vaddlv_s16( _value )};
		if constexpr( isU32 )	return ulong{ vaddlv_u32( _value )};
		if constexpr( isI32 )	return slong{ vaddlv_s32( _value )};
	}

	template <typename IT>
	__Ce__ bool  SimdTInt64<IT>::Has_ReduceAddExt ()
	{
		return sizeof(Scalar_t) <= sizeof(uint);
	}

/*
=================================================
	ReduceMaxScalar
=================================================
*/
	template <typename IT>
	IT  SimdTInt64<IT>::ReduceMaxScalar () C_NE___
	{
		StaticAssert( Has_ReduceMinMax() );
		if constexpr( isU8 )	return vmaxv_u8(  _value );
		if constexpr( isI8 )	return vmaxv_s8(  _value );
		if constexpr( isU16 )	return vmaxv_u16( _value );
		if constexpr( isI16 )	return vmaxv_s16( _value );
		if constexpr( isU32 )	return vmaxv_u32( _value );
		if constexpr( isI32 )	return vmaxv_s32( _value );
	}

	template <typename IT>
	__Ce__ bool  SimdTInt64<IT>::Has_ReduceMinMax ()
	{
		return sizeof(Scalar_t) <= sizeof(uint);
	}

/*
=================================================
	ReduceMinScalar
=================================================
*/
	template <typename IT>
	IT  SimdTInt64<IT>::ReduceMinScalar () C_NE___
	{
		StaticAssert( Has_ReduceMinMax() );
		if constexpr( isU8 )	return vminv_u8(  _value );
		if constexpr( isI8 )	return vminv_s8(  _value );
		if constexpr( isU16 )	return vminv_u16( _value );
		if constexpr( isI16 )	return vminv_s16( _value );
		if constexpr( isU32 )	return vminv_u32( _value );
		if constexpr( isI32 )	return vminv_s32( _value );
	}

#endif // AE_SIMD_NEON64

/*
=================================================
	_LShift_LogicV
=================================================
*/
	template <typename IT>
	SimdTInt64<IT>  SimdTInt64<IT>::_LShift_LogicV (const Signed_t &shift)  C_NE___
	{
		StaticAssert( Has_VecLShift_Logic() );
		ASSERT_MSG( shift.Abs().LEqual( Signed_t{CT_SizeOfInBits<Scalar_t>} ).All(), "Result will be zero" );

		if constexpr( isI8 )	return Self{ vshl_s8(  _value, shift.Ref() )};
		if constexpr( isU8 )	return Self{ vshl_u8(  _value, shift.Ref() )};
		if constexpr( isI16 )	return Self{ vshl_s16( _value, shift.Ref() )};
		if constexpr( isU16 )	return Self{ vshl_u16( _value, shift.Ref() )};
		if constexpr( isI32 )	return Self{ vshl_s32( _value, shift.Ref() )};
		if constexpr( isU32 )	return Self{ vshl_u32( _value, shift.Ref() )};
	}

/*
=================================================
	_LShift_ArithV
=================================================
*/
	template <typename IT>
	SimdTInt64<IT>  SimdTInt64<IT>::_LShift_ArithV (const Signed_t &shift)  C_NE___
	{
		StaticAssert( Has_VecLShift_Arithmetic() );
		ASSERT_MSG( shift.Abs().LEqual( Signed_t{CT_SizeOfInBits<Scalar_t>} ).All(), "Result will be zero" );

		if constexpr( isI8 )	return Self{ vqshl_s8(  _value, shift.Ref() )};
		if constexpr( isU8 )	return Self{ vqshl_u8(  _value, shift.Ref() )};
		if constexpr( isI16 )	return Self{ vqshl_s16( _value, shift.Ref() )};
		if constexpr( isU16 )	return Self{ vqshl_u16( _value, shift.Ref() )};
		if constexpr( isI32 )	return Self{ vqshl_s32( _value, shift.Ref() )};
		if constexpr( isU32 )	return Self{ vqshl_u32( _value, shift.Ref() )};
	}

/*
=================================================
	LShift_Logic
----
	returns zero element on overflow
=================================================
*/
	template <typename IT>
	template <uint Shift>
	SimdTInt64<IT>  SimdTInt64<IT>::LShift_Logic ()  C_NE___
	{
		StaticAssert( Has_ScalarShift_Logic() );
		StaticAssert( Shift <= CT_SizeOfInBits<Scalar_t> );

		if constexpr( isI8 )	return Self{ vshl_n_s8(  _value, Shift )};
		if constexpr( isU8 )	return Self{ vshl_n_u8(  _value, Shift )};
		if constexpr( isI16 )	return Self{ vshl_n_s16( _value, Shift )};
		if constexpr( isU16 )	return Self{ vshl_n_u16( _value, Shift )};
		if constexpr( isI32 )	return Self{ vshl_n_s32( _value, Shift )};
		if constexpr( isU32 )	return Self{ vshl_n_u32( _value, Shift )};
	}

/*
=================================================
	LShift_Arith
----
	returns max element on unsigned overflow
	returns -1 element on signed overflow
=================================================
*/
	template <typename IT>
	template <uint Shift>
	SimdTInt64<IT>  SimdTInt64<IT>::LShift_Arith ()  C_NE___
	{
		StaticAssert( Has_ScalarShift_Arithmetic() );
		StaticAssert( Shift <= CT_SizeOfInBits<Scalar_t> );

		if constexpr( isI8 )	return Self{ vqshl_n_s8(  _value, Shift )};
		if constexpr( isU8 )	return Self{ vqshl_n_u8(  _value, Shift )};
		if constexpr( isI16 )	return Self{ vqshl_n_s16( _value, Shift )};
		if constexpr( isU16 )	return Self{ vqshl_n_u16( _value, Shift )};
		if constexpr( isI32 )	return Self{ vqshl_n_s32( _value, Shift )};
		if constexpr( isU32 )	return Self{ vqshl_n_u32( _value, Shift )};
	}

/*
=================================================
	RShift_Logic
----
	returns zero element on overflow
=================================================
*/
	template <typename IT>
	template <uint Shift>
	SimdTInt64<IT>  SimdTInt64<IT>::RShift_Logic ()  C_NE___
	{
		StaticAssert( Has_ScalarShift_Logic() );
		StaticAssert( Shift > 0 );
		StaticAssert( Shift <= CT_SizeOfInBits<Scalar_t> );

		if constexpr( isI8 )	return Self{ vshr_n_s8(  _value, Shift )};
		if constexpr( isU8 )	return Self{ vshr_n_u8(  _value, Shift )};
		if constexpr( isI16 )	return Self{ vshr_n_s16( _value, Shift )};
		if constexpr( isU16 )	return Self{ vshr_n_u16( _value, Shift )};
		if constexpr( isI32 )	return Self{ vshr_n_s32( _value, Shift )};
		if constexpr( isU32 )	return Self{ vshr_n_u32( _value, Shift )};
	}

/*
=================================================
	RShift_Arith
----
	returns max element on unsigned overflow
	returns -1 element on signed overflow
=================================================
*/
	template <typename IT>
	template <uint Shift>
	SimdTInt64<IT>  SimdTInt64<IT>::RShift_Arith ()  C_NE___
	{
		StaticAssert( Has_ScalarShift_Arithmetic() );
		StaticAssert( Shift <= CT_SizeOfInBits<Scalar_t> );
		return RShift_Arith( Shift );
	}

/*
=================================================
	Min / Max
=================================================
*/
	template <typename IT>
	SimdTInt64<IT>  SimdTInt64<IT>::Min (const Self &rhs)  C_NE___
	{
		StaticAssert( Has_MinMax() );
		if constexpr( isU8 )	return Self{ vmin_u8(  _value, rhs._value )};
		if constexpr( isI8 )	return Self{ vmin_s8(  _value, rhs._value )};
		if constexpr( isU16 )	return Self{ vmin_u16( _value, rhs._value )};
		if constexpr( isI16 )	return Self{ vmin_s16( _value, rhs._value )};
		if constexpr( isU32 )	return Self{ vmin_u32( _value, rhs._value )};
		if constexpr( isI32 )	return Self{ vmin_s32( _value, rhs._value )};
	}

	template <typename IT>
	SimdTInt64<IT>  SimdTInt64<IT>::Max (const Self &rhs)  C_NE___
	{
		StaticAssert( Has_MinMax() );
		if constexpr( isU8 )	return Self{ vmax_u8(  _value, rhs._value )};
		if constexpr( isI8 )	return Self{ vmax_s8(  _value, rhs._value )};
		if constexpr( isU16 )	return Self{ vmax_u16( _value, rhs._value )};
		if constexpr( isI16 )	return Self{ vmax_s16( _value, rhs._value )};
		if constexpr( isU32 )	return Self{ vmax_u32( _value, rhs._value )};
		if constexpr( isI32 )	return Self{ vmax_s32( _value, rhs._value )};
	}

	template <typename IT>
	__Ce__ bool  SimdTInt64<IT>::Has_MinMax ()
	{
		return sizeof(Scalar_t) <= sizeof(uint);
	}

/*
=================================================
	IsZero
=================================================
*/
	template <typename IT>
	typename SimdTInt64<IT>::Bool_t  SimdTInt64<IT>::IsZero () C_NE___
	{
	  #if AE_SIMD_NEON64
		if constexpr( isU8 )	return Bool_t{ vceqz_u8(  _value )};
		if constexpr( isI8 )	return Bool_t{ vceqz_s8(  _value )};
		if constexpr( isU16 )	return Bool_t{ vceqz_u16( _value )};
		if constexpr( isI16 )	return Bool_t{ vceqz_s16( _value )};
		if constexpr( isU32 )	return Bool_t{ vceqz_u32( _value )};
		if constexpr( isI32 )	return Bool_t{ vceqz_s32( _value )};
	  #else
		return Equal( Self{} );
	  #endif
	}

/*
=================================================
	Equal
=================================================
*/
	template <typename IT>
	typename SimdTInt64<IT>::Bool_t  SimdTInt64<IT>::Equal (const Self &rhs)  C_NE___
	{
		StaticAssert( Has_Equal() );
		if constexpr( isU8 )	return Bool_t{ vceq_u8(  _value, rhs._value )};
		if constexpr( isI8 )	return Bool_t{ vceq_s8(  _value, rhs._value )};
		if constexpr( isU16 )	return Bool_t{ vceq_u16( _value, rhs._value )};
		if constexpr( isI16 )	return Bool_t{ vceq_s16( _value, rhs._value )};
		if constexpr( isU32 )	return Bool_t{ vceq_u32( _value, rhs._value )};
		if constexpr( isI32 )	return Bool_t{ vceq_s32( _value, rhs._value )};
	}

/*
=================================================
	Greater
=================================================
*/
	template <typename IT>
	typename SimdTInt64<IT>::Bool_t  SimdTInt64<IT>::Greater (const Self &rhs)  C_NE___
	{
		StaticAssert( Has_Greater() );
		if constexpr( isU8 )	return Bool_t{ vcgt_u8(  _value, rhs._value )};
		if constexpr( isI8 )	return Bool_t{ vcgt_s8(  _value, rhs._value )};
		if constexpr( isU16 )	return Bool_t{ vcgt_u16( _value, rhs._value )};
		if constexpr( isI16 )	return Bool_t{ vcgt_s16( _value, rhs._value )};
		if constexpr( isU32 )	return Bool_t{ vcgt_u32( _value, rhs._value )};
		if constexpr( isI32 )	return Bool_t{ vcgt_s32( _value, rhs._value )};
	}

/*
=================================================
	Less
=================================================
*/
	template <typename IT>
	typename SimdTInt64<IT>::Bool_t  SimdTInt64<IT>::Less (const Self &rhs)  C_NE___
	{
		StaticAssert( Has_Greater() );
		if constexpr( isU8 )	return Bool_t{ vclt_u8(  _value, rhs._value )};
		if constexpr( isI8 )	return Bool_t{ vclt_s8(  _value, rhs._value )};
		if constexpr( isU16 )	return Bool_t{ vclt_u16( _value, rhs._value )};
		if constexpr( isI16 )	return Bool_t{ vclt_s16( _value, rhs._value )};
		if constexpr( isU32 )	return Bool_t{ vclt_u32( _value, rhs._value )};
		if constexpr( isI32 )	return Bool_t{ vclt_s32( _value, rhs._value )};
	}

/*
=================================================
	LEqual
=================================================
*/
	template <typename IT>
	typename SimdTInt64<IT>::Bool_t  SimdTInt64<IT>::LEqual (const Self &rhs)  C_NE___
	{
		StaticAssert( Has_Greater() );
		if constexpr( isU8 )	return Bool_t{ vcle_u8(  _value, rhs._value )};
		if constexpr( isI8 )	return Bool_t{ vcle_s8(  _value, rhs._value )};
		if constexpr( isU16 )	return Bool_t{ vcle_u16( _value, rhs._value )};
		if constexpr( isI16 )	return Bool_t{ vcle_s16( _value, rhs._value )};
		if constexpr( isU32 )	return Bool_t{ vcle_u32( _value, rhs._value )};
		if constexpr( isI32 )	return Bool_t{ vcle_s32( _value, rhs._value )};
	}

/*
=================================================
	GEqual
=================================================
*/
	template <typename IT>
	typename SimdTInt64<IT>::Bool_t  SimdTInt64<IT>::GEqual (const Self &rhs)  C_NE___
	{
		StaticAssert( Has_Greater() );
		if constexpr( isU8 )	return Bool_t{ vcge_u8(  _value, rhs._value )};
		if constexpr( isI8 )	return Bool_t{ vcge_s8(  _value, rhs._value )};
		if constexpr( isU16 )	return Bool_t{ vcge_u16( _value, rhs._value )};
		if constexpr( isI16 )	return Bool_t{ vcge_s16( _value, rhs._value )};
		if constexpr( isU32 )	return Bool_t{ vcge_u32( _value, rhs._value )};
		if constexpr( isI32 )	return Bool_t{ vcge_s32( _value, rhs._value )};
	}

/*
=================================================
	ToBitfield
=================================================
*/
	template <typename IT>
	template <typename T> requires( IsUnsignedInteger<T> )
	typename SimdTInt64<IT>::Mask_t  SimdTInt64<IT>::ToBitfield ()  C_NE___
	{
		if constexpr( isU8 )
		{
			const sbyte		shift[] = {0,1,2,3,4,5,6,7};
			auto	a = vshr_n_u8( _value, 7 );
			auto	b = vshl_u8( a, vld1_s8(shift) );
		  #if AE_SIMD_NEON64
			return Mask_t{ vaddv_u8( b )};
		  #else
			uint	v[2];
			vst1_u32( OUT v, vreinterpret_u32_u8( b ));
			uint	u = v[0] | v[1];
			return Mask_t{ (u | (u >> 8) | (u >> 16) | (u >> 24)) & 0xFF };
		  #endif
		}
		if constexpr( isU16 )
		{
			const sshort	shift[] = {0,1,2,3};
			auto	a = vshr_n_u16( _value, 15 );
			auto	b = vshl_u16( a, vld1_s16(shift) );
		  #if AE_SIMD_NEON64
			return Mask_t{ vaddv_u16( b )};
		  #else
			uint	v[2];
			vst1_u32( OUT v, vreinterpret_u32_u16( b ));
			uint	u = v[0] | v[1];
			return Mask_t{ (u | (u >> 16)) & 0xFFFF };
		  #endif
		}
		if constexpr( isU32 )
		{
			const sint		shift[] = {0,1};
			auto	a = vshr_n_u32( _value, 31 );
			auto	b = vshl_u32( a, vld1_s32(shift) );
		  #if AE_SIMD_NEON64
			return Mask_t{ vaddv_u32( b )};
		  #else
			uint	v[2];
			vst1_u32( OUT v, b );
			return Mask_t{ v[0] | v[1] };
		  #endif
		}
	}

/*
=================================================
	All
=================================================
*/
	template <typename IT>
	template <typename T> requires( IsUnsignedInteger<T> )
	bool  SimdTInt64<IT>::All ()  C_NE___
	{
	#if AE_SIMD_NEON64
		if constexpr( isU8  )	return vaddv_u8(  vshr_n_u8(  _value, 3 )) == (0xFF >> 3)*8;
		if constexpr( isU16 )	return vaddv_u16( vshr_n_u16( _value, 2 )) == (0xFFFF >> 2)*4;
		if constexpr( isU32 )	return vaddv_u32( vshr_n_u32( _value, 1 )) == (0xFFFF'FFFF >> 1)*2;
	#else
		if constexpr( isU8  )	return vget_lane_u64( vreinterpret_u64_u8(  _value ), 0 ) == UMax;
		if constexpr( isU16 )	return vget_lane_u64( vreinterpret_u64_u16( _value ), 0 ) == UMax;
		if constexpr( isU32 )	return vget_lane_u64( vreinterpret_u64_u32( _value ), 0 ) == UMax;
	#endif
	}

/*
=================================================
	Any
=================================================
*/
	template <typename IT>
	template <typename T> requires( IsUnsignedInteger<T> )
	bool  SimdTInt64<IT>::Any ()  C_NE___
	{
	#if AE_SIMD_NEON64
		if constexpr( isU8  )	return vaddv_u8(  vshr_n_u8(  _value, 3 )) != 0;
		if constexpr( isU16 )	return vaddv_u16( vshr_n_u16( _value, 2 )) != 0;
		if constexpr( isU32 )	return vaddv_u32( vshr_n_u32( _value, 1 )) != 0;
	#else
		if constexpr( isU8  )	return vget_lane_u64( vreinterpret_u64_u8(  _value ), 0 ) != 0;
		if constexpr( isU16 )	return vget_lane_u64( vreinterpret_u64_u16( _value ), 0 ) != 0;
		if constexpr( isU32 )	return vget_lane_u64( vreinterpret_u64_u32( _value ), 0 ) != 0;
	#endif
	}

/*
=================================================
	None
=================================================
*/
	template <typename IT>
	template <typename T> requires( IsUnsignedInteger<T> )
	bool  SimdTInt64<IT>::None ()  C_NE___
	{
	#if AE_SIMD_NEON64
		if constexpr( isU8  )	return vaddv_u8(  vshr_n_u8(  _value, 3 )) == 0;
		if constexpr( isU16 )	return vaddv_u16( vshr_n_u16( _value, 2 )) == 0;
		if constexpr( isU32 )	return vaddv_u32( vshr_n_u32( _value, 1 )) == 0;
	#else
		if constexpr( isU8  )	return vget_lane_u64( vreinterpret_u64_u8(  _value ), 0 ) == 0;
		if constexpr( isU16 )	return vget_lane_u64( vreinterpret_u64_u16( _value ), 0 ) == 0;
		if constexpr( isU32 )	return vget_lane_u64( vreinterpret_u64_u32( _value ), 0 ) == 0;
	#endif
	}

/*
=================================================
	MulAdd
=================================================
*/
	template <typename IT>
	ND_ SimdTInt64<IT>  MulAdd (const SimdTInt64<IT> &a, const SimdTInt64<IT> &b, const SimdTInt64<IT> &c) __NE___
	{
		StaticAssert( SimdTInt64<IT>::Has_MulAdd );
		if constexpr( IsSame< IT, sshort >)	return SimdTInt64<IT>{ vmla_s16( c._value, a._value, b._value )};
		if constexpr( IsSame< IT, ushort >)	return SimdTInt64<IT>{ vmla_u16( c._value, a._value, b._value )};
		if constexpr( IsSame< IT, sint   >)	return SimdTInt64<IT>{ vmla_s32( c._value, a._value, b._value )};
		if constexpr( IsSame< IT, uint   >)	return SimdTInt64<IT>{ vmla_u32( c._value, a._value, b._value )};
	}

	template <typename IT>
	ND_ SimdTInt64<IT>  MulAdd (const SimdTInt64<IT> &a, IT b, const SimdTInt64<IT> &c) __NE___
	{
		StaticAssert( SimdTInt64<IT>::Has_MulAdd );
		if constexpr( IsSame< IT, sshort >)	return SimdTInt64<IT>{ vmla_n_s16( c._value, a._value, b )};
		if constexpr( IsSame< IT, ushort >)	return SimdTInt64<IT>{ vmla_n_u16( c._value, a._value, b )};
		if constexpr( IsSame< IT, sint   >)	return SimdTInt64<IT>{ vmla_n_s32( c._value, a._value, b )};
		if constexpr( IsSame< IT, uint   >)	return SimdTInt64<IT>{ vmla_n_u32( c._value, a._value, b )};
	}

/*
=================================================
	NegMulAdd
=================================================
*/
	template <typename IT>
	ND_ SimdTInt64<IT>  NegMulAdd (const SimdTInt64<IT> &a, const SimdTInt64<IT> &b, const SimdTInt64<IT> &c) __NE___
	{
		StaticAssert( SimdTInt64<IT>::Has_MulAdd );
		if constexpr( IsSame< IT, sbyte  >)	return SimdTInt64<IT>{ vmls_s8(  c._value, a._value, b._value )};
		if constexpr( IsSame< IT, ubyte  >)	return SimdTInt64<IT>{ vmls_u8(  c._value, a._value, b._value )};
		if constexpr( IsSame< IT, sshort >)	return SimdTInt64<IT>{ vmls_s16( c._value, a._value, b._value )};
		if constexpr( IsSame< IT, ushort >)	return SimdTInt64<IT>{ vmls_u16( c._value, a._value, b._value )};
		if constexpr( IsSame< IT, sint   >)	return SimdTInt64<IT>{ vmls_s32( c._value, a._value, b._value )};
		if constexpr( IsSame< IT, uint   >)	return SimdTInt64<IT>{ vmls_u32( c._value, a._value, b._value )};
	}

	template <typename IT>
	ND_ SimdTInt64<IT>  NegMulAdd (const SimdTInt64<IT> &a, IT b, const SimdTInt64<IT> &c) __NE___
	{
		StaticAssert( SimdTInt64<IT>::Has_MulAdd );
		if constexpr( IsSame< IT, sshort >)	return SimdTInt64<IT>{ vmls_n_s16( c._value, a._value, b )};
		if constexpr( IsSame< IT, ushort >)	return SimdTInt64<IT>{ vmls_n_u16( c._value, a._value, b )};
		if constexpr( IsSame< IT, sint   >)	return SimdTInt64<IT>{ vmls_n_s32( c._value, a._value, b )};
		if constexpr( IsSame< IT, uint   >)	return SimdTInt64<IT>{ vmls_n_u32( c._value, a._value, b )};
	}

/*
=================================================
	Select
=================================================
*/
	template <typename IT>
	SimdTInt64<IT>  SimdTInt64<IT>::_Select (const typename SimdTInt64<IT>::Bool_t &condition, const SimdTInt64<IT> &ifTrue, const SimdTInt64<IT> &ifFalse) __NE___
	{
		if constexpr( IsSame< IT, sbyte  >)	return SimdTInt64<IT>{ vbsl_s8(  condition.Ref(), ifTrue._value, ifFalse._value )};
		if constexpr( IsSame< IT, ubyte  >)	return SimdTInt64<IT>{ vbsl_u8(  condition.Ref(), ifTrue._value, ifFalse._value )};
		if constexpr( IsSame< IT, sshort >)	return SimdTInt64<IT>{ vbsl_s16( condition.Ref(), ifTrue._value, ifFalse._value )};
		if constexpr( IsSame< IT, ushort >)	return SimdTInt64<IT>{ vbsl_u16( condition.Ref(), ifTrue._value, ifFalse._value )};
		if constexpr( IsSame< IT, sint   >)	return SimdTInt64<IT>{ vbsl_s32( condition.Ref(), ifTrue._value, ifFalse._value )};
		if constexpr( IsSame< IT, uint   >)	return SimdTInt64<IT>{ vbsl_u32( condition.Ref(), ifTrue._value, ifFalse._value )};
	}

/*
=================================================
	ToArray
=================================================
*/
	template <typename IT>
	void  SimdTInt64<IT>::ToArray (OUT Scalar_t* dst)  C_NE___
	{
		NonNull( dst );
		if constexpr( isI8 )	vst1_s8(  OUT dst, _value );
		if constexpr( isU8 )	vst1_u8(  OUT dst, _value );
		if constexpr( isI16 )	vst1_s16( OUT dst, _value );
		if constexpr( isU16 )	vst1_u16( OUT dst, _value );
		if constexpr( isI32 )	vst1_s32( OUT dst, _value );
		if constexpr( isU32 )	vst1_u32( OUT dst, _value );
	}

/*
=================================================
	Swizzle (Shoft4)
=================================================
*/
	template <typename IT>
	template <uint X, uint Y, uint Z, uint W,  typename T> requires( sizeof(T)==2 )
	SimdTInt64<IT>  SimdTInt64<IT>::Swizzle () C_NE___
	{
		StaticAssert( Has_Swizzle() );
		StaticAssert( X < count );
		StaticAssert( Y < count );
		StaticAssert( Z < count );
		StaticAssert( W < count );
		return Self{ __builtin_shufflevector( _value, _value, X, Y, Z, W )};
	}

/*
=================================================
	Swizzle (Int2)
=================================================
*/
	template <typename IT>
	template <uint X, uint Y,  typename T> requires( sizeof(T)==4 )
	SimdTInt64<IT>  SimdTInt64<IT>::Swizzle ()  C_NE___
	{
		StaticAssert( Has_Swizzle() );
		StaticAssert( X < count );
		StaticAssert( Y < count );
		return Self{ __builtin_shufflevector( _value, _value, X, Y )};
	}

/*
=================================================
	ToSigned
=================================================
*/
	template <typename IT>
	template <typename T> requires( IsUnsignedInteger<T> )
	auto  SimdTInt64<IT>::ToSigned () C_NE___
	{
		using S = ToSignedInteger<IT>;
		return BitCast< SimdTInt64<S> >().Max( S{0} );
	}

/*
=================================================
	ToUnsigned
=================================================
*/
	template <typename IT>
	template <typename T> requires( IsSignedInteger<T> )
	auto  SimdTInt64<IT>::ToUnsigned () C_NE___
	{
		using U = ToUnsignedInteger<IT>;
		return Max( IT{0} ).template BitCast< SimdTInt64<U> >();
	}

/*
=================================================
	ToHalf
=================================================
*/
# ifdef AE_SIMD_SimdHalf4
	template <typename IT>
	auto  SimdTInt64<IT>::ToHalf () C_NE___
	{
		if constexpr( isI16 )
			return SimdHalf4{ vcvt_f16_s16( _value )};
		else
		if constexpr( isU16 )
			return SimdHalf4{ vcvt_f16_u16( _value )};
		else
		if constexpr( isI32 or isU32 )
			return ToFloat().ToHalf();
		else
		if constexpr( isI8 or isU8 )
			return ToShort().ToHalf();
	}
# endif
/*
=================================================
	ToFloat
=================================================
*/
	template <typename IT>
	template <uint Idx>
	auto  SimdTInt64<IT>::ToFloat () C_NE___
	{
	  #ifdef AE_SIMD_SimdFloat2
		if constexpr( isI32 )
		{
			StaticAssert( Idx == 0 );
			return SimdFloat2{ vcvt_f32_s32( _value )};
		}else
		if constexpr( isU32 )
		{
			StaticAssert( Idx == 0 );
			return SimdFloat2{ vcvt_f32_u32( _value )};
		}else

	  #elif defined(AE_SIMD_SimdFloat4)
		if constexpr( isI32 )
		{
			StaticAssert( Idx == 0 );
			float32x2_t	low		= vcvt_f32_s32( _value );
			float32x2_t	high	= vdup_n_f32( 0.f );
			return SimdFloat4{ vcombine_f32( low, high )};
		}else
		if constexpr( isU32 )
		{
			StaticAssert( Idx == 0 );
			float32x2_t	low		= vcvt_f32_u32( _value );
			float32x2_t	high	= vdup_n_f32( 0.f );
			return SimdFloat4{ vcombine_f32( low, high )};
		}else
	  #endif

			return ToInt<Idx>().ToFloat();
	}

/*
=================================================
	ToByte
=================================================
*/
	template <typename IT>
	auto  SimdTInt64<IT>::ToByte () C_NE___
	{
		StaticAssert( sizeof(Scalar_t) != sizeof(ubyte) );

		if constexpr( isI16 )
			return SimdByte8{ vmovn_s16( vcombine_s16( _value, _value ))};
		else
		if constexpr( isU16 )
			return SimdUByte8{ vmovn_u16( vcombine_u16( _value, _value ))};
		else
		if constexpr( isI32 or isU32 )
			return ToShort().ToByte();
	}

/*
=================================================
	ToShort
=================================================
*/
	template <typename IT>
	auto  SimdTInt64<IT>::ToShort () C_NE___
	{
		StaticAssert( sizeof(Scalar_t) != sizeof(ushort) );

		if constexpr( isI8 )
			return SimdShort8{ vmovl_s8( _value )};
		else
		if constexpr( isU8 )
			return SimdUShort8{ vmovl_u8( _value )};
		else
		if constexpr( isI32 )
			return SimdShort4{ vmovn_s32( vcombine_s32( _value, vdup_n_s32(0) ))};
		else
		if constexpr( isU32 )
			return SimdUShort4{ vmovn_u32( vcombine_u32( _value, vdup_n_u32(0) ))};
	}

/*
=================================================
	ToInt
=================================================
*/
	template <typename IT>
	template <uint Idx>
	auto  SimdTInt64<IT>::ToInt () C_NE___
	{
		StaticAssert( sizeof(Scalar_t) != sizeof(uint) );

		if constexpr( isI16 )
		{
			StaticAssert( Idx == 0 );
			return SimdInt4{ vmovl_s16( _value )};
		}else
		if constexpr( isU16 )
		{
			StaticAssert( Idx == 0 );
			return SimdUInt4{ vmovl_u16( _value )};
		}else
		if constexpr( isU8 or isI8 )
		{
			StaticAssert( Idx < 2 );
			return ToShort().template ToInt< Idx >();
		}
	}

/*
=================================================
	ToLong
=================================================
*/
	template <typename IT>
	template <uint Idx>
	auto  SimdTInt64<IT>::ToLong () C_NE___
	{
		StaticAssert( sizeof(Scalar_t) != sizeof(ulong) );

		if constexpr( isI32 )
		{
			StaticAssert( Idx == 0 );
			return SimdLong2{ vmovl_s32( _value )};
		}else
		if constexpr( isU32 )
		{
			StaticAssert( Idx == 0 );
			return SimdULong2{ vmovl_u32( _value )};
		}else
		if constexpr( isI16 or isU16 )
		{
			StaticAssert( Idx < 2 );
			return ToInt().template ToLong< Idx >();
		}else
		if constexpr( isU8 or isI8 )
		{
			StaticAssert( Idx < 4 );
			return ToInt< Idx/2 >().template ToLong< Idx&1 >();
		}
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
	auto  SimdTInt64<IT>::Convert ()  C_NE___
	{
		StaticAssert( IsAnyScalar< DstScalar >);
		StaticAssert( not IsSame< DstScalar, Scalar_t >);
		StaticAssert( Has_Convert< DstScalar >() );

	  #ifdef AE_SIMD_SimdHalf4
		if constexpr( IsSame< DstScalar, half >)
		{
			if constexpr( isI8  or isU8  )	return ToHalf();	// byte8 -> half8
			if constexpr( isI16 or isU16 )	return ToHalf();	// short4 -> half4
			if constexpr( isI32 or isU32 )	return ToHalf();	// int2 -> half4
		}
	  #endif
	  #ifdef AE_SIMD_SimdFloat4
		if constexpr( IsSame< DstScalar, float >)
		{
			if constexpr( isI8  or isU8  )	return std::array{ ToFloat<0>(), ToFloat<1>() };
			if constexpr( isI16 or isU16 )	return ToFloat();
			if constexpr( isI32 or isU32 )	return ToFloat();
		}
	  #endif
	  #ifdef AE_SIMD_SimdFloat2
		if constexpr( IsSame< DstScalar, float >)
		{
			if constexpr( isI8  or isU8  )	return std::array{ ToFloat<0>(), ToFloat<1>(), ToFloat<2>(), ToFloat<3>() };
			if constexpr( isI16 or isU16 )	return std::array{ ToFloat<0>(), ToFloat<1>() };
			if constexpr( isI32 or isU32 )	return ToFloat();
		}
	  #endif

		if constexpr( IsInteger< DstScalar >)
		{
			using Dst  = SimdTInt64<DstScalar>;
			using Dst2 = SimdTInt128<DstScalar>;

			if constexpr( IsSame< DstScalar, ulong >)
			{
				if constexpr( isU8 )	return std::array{	ToLong<0>(), ToLong<1>(), ToLong<2>(), ToLong<3>() };
				if constexpr( isI8 )	return std::array{	ToLong<0>().template BitCast<Dst2>(), ToLong<1>().template BitCast<Dst2>(),
															ToLong<2>().template BitCast<Dst2>(), ToLong<3>().template BitCast<Dst2>() };
				if constexpr( isU16 )	return std::array{	ToLong<0>(), ToLong<1>() };
				if constexpr( isI16 )	return std::array{	ToLong<0>().template BitCast<Dst2>(), ToLong<1>().template BitCast<Dst2>() };
				if constexpr( isU32 )	return ToLong();
				if constexpr( isI32 )	return ToLong().template BitCast<Dst2>();
			}
			if constexpr( IsSame< DstScalar, slong >)
			{
				if constexpr( isI8 )	return std::array{	ToLong<0>(), ToLong<1>(), ToLong<2>(), ToLong<3>() };
				if constexpr( isU8 )	return std::array{	ToLong<0>().template BitCast<Dst2>(), ToLong<1>().template BitCast<Dst2>(),
															ToLong<2>().template BitCast<Dst2>(), ToLong<3>().template BitCast<Dst2>()};
				if constexpr( isI16 )	return std::array{	ToLong<0>(), ToLong<1>() };
				if constexpr( isU16 )	return std::array{	ToLong<0>().template BitCast<Dst2>(), ToLong<1>().template BitCast<Dst2>() };
				if constexpr( isI32 )	return ToLong();
				if constexpr( isU32 )	return ToLong().template BitCast<Dst2>();
			}

			if constexpr( IsSame< DstScalar, uint >)
			{
				if constexpr( isU8 )	return std::array{ ToInt<0>(), ToInt<1>() };
				if constexpr( isI8 )	return std::array{ ToInt<0>().template BitCast<Dst2>(), ToInt<1>().template BitCast<Dst2>() };
				if constexpr( isU16 )	return ToInt();
				if constexpr( isI16 )	return ToInt().template BitCast<Dst2>();
				if constexpr( isI32 )	return BitCast<Dst>();
			}
			if constexpr( IsSame< DstScalar, sint >)
			{
				if constexpr( isI8 )	return std::array{ ToInt<0>(), ToInt<1>() };
				if constexpr( isU8 )	return std::array{ ToInt<0>().template BitCast<Dst2>(), ToInt<1>().template BitCast<Dst2>() };
				if constexpr( isI16 )	return ToInt();
				if constexpr( isU16 )	return ToInt().template BitCast<Dst2>();
				if constexpr( isU32 )	return BitCast<Dst>();
			}

			if constexpr( IsSame< DstScalar, ushort >)
			{
				if constexpr( isU8  )	return ToShort();
				if constexpr( isI8  )	return ToShort().template BitCast<Dst2>();
				if constexpr( isI16 )	return BitCast<Dst>();
				if constexpr( isU32 )	return ToShort();
				if constexpr( isI32 )	return ToShort().template BitCast<Dst>();
			}
			if constexpr( IsSame< DstScalar, sshort >)
			{
				if constexpr( isI8  )	return ToShort();
				if constexpr( isU8  )	return ToShort().template BitCast<Dst2>();
				if constexpr( isU16 )	return BitCast<Dst>();
				if constexpr( isI32 )	return ToShort();
				if constexpr( isU32 )	return ToShort().template BitCast<Dst>();
			}

			if constexpr( IsSame< DstScalar, ubyte >)
			{
				if constexpr( isI8 )			return BitCast<Dst>();
				if constexpr( isU16 or isU32 )	return ToByte();
				if constexpr( isI16 or isI32 )	return ToByte().template BitCast<Dst>();
			}
			if constexpr( IsSame< DstScalar, sbyte >)
			{
				if constexpr( isU8 )			return BitCast<Dst>();
				if constexpr( isU16 or isU32 )	return ToByte().template BitCast<Dst>();
				if constexpr( isI16 or isI32 )	return ToByte();
			}
		}
	}

/*
=================================================
	Has_Convert
=================================================
*/
	template <typename IT>
	template <typename DstScalar>
	__Ce__ bool  SimdTInt64<IT>::Has_Convert ()
	{
		if constexpr( IsSame< DstScalar, Scalar_t >)
			return false;
		else
	  #ifdef AE_SIMD_SimdHalf4
		if constexpr( IsSame< DstScalar, half >)
			return true;
		else
	  #endif
	  #if defined(AE_SIMD_SimdFloat2) or defined(AE_SIMD_SimdFloat4)
		if constexpr( IsSame< DstScalar, float >)
			return true;
		else
	  #endif
		if constexpr( IsInteger< DstScalar >)
			return true;
		else
			return false;
	}

/*
=================================================
	BitCast
=================================================
*/
	template <typename IT>
	template <typename DstType>
	DstType  SimdTInt64<IT>::BitCast ()  C_NE___
	{
		StaticAssert( sizeof(Self) == sizeof(DstType) );
		StaticAssert( Has_BitCast<DstType>() );

		if constexpr( isI8 )
		{
			if constexpr( IsSame< DstType, ulong >)			return vget_lane_u64( vreinterpret_u64_s8( _value ), 0 );
			if constexpr( IsSame< DstType, slong >)			return vget_lane_s64( vreinterpret_s64_s8( _value ), 0 );
			if constexpr( IsSame< DstType, SimdUByte8 >)	return SimdUByte8{ vreinterpret_u8_s8( _value )};
		}
		if constexpr( isU8 )
		{
			if constexpr( IsSame< DstType, ulong >)			return vget_lane_u64( vreinterpret_u64_u8( _value ), 0 );
			if constexpr( IsSame< DstType, slong >)			return vget_lane_s64( vreinterpret_s64_u8( _value ), 0 );
			if constexpr( IsSame< DstType, SimdByte8 >)		return SimdByte8{ vreinterpret_u8_s8( _value )};
		}
		if constexpr( isI16 )
		{
			if constexpr( IsSame< DstType, ulong >)			return vget_lane_u64( vreinterpret_u64_s16( _value ), 0 );
			if constexpr( IsSame< DstType, slong >)			return vget_lane_s64( vreinterpret_s64_s16( _value ), 0 );
			if constexpr( IsSame< DstType, SimdUShort4 >)	return SimdUShort4{ vreinterpret_u16_s16( _value )};
		  #ifdef AE_SIMD_SimdHalf4
			if constexpr( IsSame< DstType, SimdHalf4 >)		return SimdHalf4{ vreinterpret_f16_s16( _value )};
		  #endif
		}
		if constexpr( isU16 )
		{
			if constexpr( IsSame< DstType, ulong >)			return vget_lane_u64( vreinterpret_u64_u16( _value ), 0 );
			if constexpr( IsSame< DstType, slong >)			return vget_lane_s64( vreinterpret_s64_u16( _value ), 0 );
			if constexpr( IsSame< DstType, SimdShort4 >)	return SimdShort4{ vreinterpret_s16_u16( _value )};
		  #ifdef AE_SIMD_SimdHalf4
			if constexpr( IsSame< DstType, SimdHalf4 >)		return SimdHalf4{ vreinterpret_f16_u16( _value )};
		  #endif
		}
		if constexpr( isI32 )
		{
			if constexpr( IsSame< DstType, ulong >)			return vget_lane_u64( vreinterpret_u64_s32( _value ), 0 );
			if constexpr( IsSame< DstType, slong >)			return vget_lane_s64( vreinterpret_s64_s32( _value ), 0 );
			if constexpr( IsSame< DstType, SimdUInt2 >)		return SimdUInt2{ vreinterpret_u32_s32( _value )};
		}
		if constexpr( isU32 )
		{
			if constexpr( IsSame< DstType, ulong >)			return vget_lane_u64( vreinterpret_u64_u32( _value ), 0 );
			if constexpr( IsSame< DstType, slong >)			return vget_lane_s64( vreinterpret_s64_u32( _value ), 0 );
			if constexpr( IsSame< DstType, SimdInt2 >)		return SimdInt2{ vreinterpret_s32_u32( _value )};
		}
	}

/*
=================================================
	Has_BitCast
=================================================
*/
	template <typename IT>
	template <typename DstType>
	__Ce__ bool  SimdTInt64<IT>::Has_BitCast ()
	{
		if constexpr( IsInteger<DstType> and sizeof(DstType) == sizeof(Self) )
			return true;
		else
		if constexpr( IsSame< DstType, SimdHalf4 > and sizeof(IT) == sizeof(half) )
			return true;
		else
		if constexpr( IsSpecializationOf< DstType, SimdTInt64 >)
			return true;
		else
			return false;
	}

#endif // AE_SIMD_SimdTInt64
//-----------------------------------------------------------------------------



#ifdef AE_SIMD_SimdTInt128
/*
=================================================
	constructor
=================================================
*/
	template <typename IT>
	SimdTInt128<IT>::SimdTInt128 (IT val)  __NE___
	{
		if constexpr( isU8 )	_value = vdupq_n_u8(  val );
		if constexpr( isI8 )	_value = vdupq_n_s8(  val );
		if constexpr( isU16 )	_value = vdupq_n_u16( val );
		if constexpr( isI16 )	_value = vdupq_n_s16( val );
		if constexpr( isU32 )	_value = vdupq_n_u32( val );
		if constexpr( isI32 )	_value = vdupq_n_s32( val );
		if constexpr( isU64 )	_value = vdupq_n_u64( val );
		if constexpr( isI64 )	_value = vdupq_n_s64( val );
	}

	template <typename IT>
	SimdTInt128<IT>::SimdTInt128 (const IT* ptr) __NE___
	{
		NonNull( ptr );
		if constexpr( isU8 )	_value = vld1q_u8(  ptr );
		if constexpr( isI8 )	_value = vld1q_s8(  ptr );
		if constexpr( isU16 )	_value = vld1q_u16( ptr );
		if constexpr( isI16 )	_value = vld1q_s16( ptr );
		if constexpr( isU32 )	_value = vld1q_u32( ptr );
		if constexpr( isI32 )	_value = vld1q_s32( ptr );
		if constexpr( isU64 )	_value = vld1q_u64( ptr );
		if constexpr( isI64 )	_value = vld1q_s64( ptr );
	}

/*
=================================================
	constructor
=================================================
*/
	template <typename IT>
	template <typename T> requires( sizeof(T)==1 )
	SimdTInt128<IT>::SimdTInt128 (Scalar_t v00, Scalar_t v01, Scalar_t v02, Scalar_t v03,
								  Scalar_t v04, Scalar_t v05, Scalar_t v06, Scalar_t v07,
								  Scalar_t v08, Scalar_t v09, Scalar_t v10, Scalar_t v11,
								  Scalar_t v12, Scalar_t v13, Scalar_t v14, Scalar_t v15) __NE___
	{
		Scalar_t a[] = {v00, v01, v02, v03, v04, v05, v06, v07, v08, v09, v10, v11, v12, v13, v14, v15};
		StaticAssert( CountOf(a) == 16 );
		if constexpr( IsSigned<IT> )
			_value = vld1q_s8( a );
		else
			_value = vld1q_u8( a );
	}

	template <typename IT>
	template <typename T> requires( sizeof(T)==2 )
	SimdTInt128<IT>::SimdTInt128 (Scalar_t v0, Scalar_t v1, Scalar_t v2, Scalar_t v3,
								  Scalar_t v4, Scalar_t v5, Scalar_t v6, Scalar_t v7) __NE___
	{
		Scalar_t a[] = {v0, v1, v2, v3, v4, v5, v6, v7};
		StaticAssert( CountOf(a) == 8 );
		if constexpr( IsSigned<IT> )
			_value = vld1q_s16( a );
		else
			_value = vld1q_u16( a );
	}

	template <typename IT>
	template <typename T> requires( sizeof(T)==4 )
	SimdTInt128<IT>::SimdTInt128 (Scalar_t v0, Scalar_t v1, Scalar_t v2, Scalar_t v3) __NE___
	{
		Scalar_t a[] = {v0, v1, v2, v3};
		StaticAssert( CountOf(a) == 4 );
		if constexpr( IsSigned<IT> )
			_value = vld1q_s32( a );
		else
			_value = vld1q_u32( a );
	}

	template <typename IT>
	template <typename T> requires( sizeof(T)==8 )
	SimdTInt128<IT>::SimdTInt128 (Scalar_t v0, Scalar_t v1) __NE___
	{
		Scalar_t a[] = {v0, v1};
		if constexpr( IsSigned<IT> )
			_value = vld1q_s64( a );
		else
			_value = vld1q_u64( a );
	}

/*
=================================================
	constructor
=================================================
*/
	template <typename IT>
	template <typename B, typename T> requires( IsSame< T, ubyte > and IsSame< B, bool >)
	SimdTInt128<IT>::SimdTInt128 (B v00, B v01, B v02, B v03, B v04, B v05, B v06, B v07,
								  B v08, B v09, B v10, B v11, B v12, B v13, B v14, B v15) __NE___
	{
		ubyte  a[] = {	T(v00 ? 0xFF : 0), T(v01 ? 0xFF : 0), T(v02 ? 0xFF : 0), T(v03 ? 0xFF : 0),
						T(v04 ? 0xFF : 0), T(v05 ? 0xFF : 0), T(v06 ? 0xFF : 0), T(v07 ? 0xFF : 0),
						T(v08 ? 0xFF : 0), T(v09 ? 0xFF : 0), T(v10 ? 0xFF : 0), T(v11 ? 0xFF : 0),
						T(v12 ? 0xFF : 0), T(v13 ? 0xFF : 0), T(v14 ? 0xFF : 0), T(v15 ? 0xFF : 0) };
		StaticAssert( CountOf(a) == 16 );
		_value = vld1q_u8( a );
	}

	template <typename IT>
	template <typename B, typename T> requires( IsSame< T, ushort > and IsSame< B, bool >)
	SimdTInt128<IT>::SimdTInt128 (B v0, B v1, B v2, B v3, B v4, B v5, B v6, B v7) __NE___
	{
		ushort  a[] = { T(v0 ? 0xFFFF : 0), T(v1 ? 0xFFFF : 0), T(v2 ? 0xFFFF : 0), T(v3 ? 0xFFFF : 0),
						T(v4 ? 0xFFFF : 0), T(v5 ? 0xFFFF : 0), T(v6 ? 0xFFFF : 0), T(v7 ? 0xFFFF : 0) };
		StaticAssert( CountOf(a) == 8 );
		_value = vld1q_u16( a );
	}

	template <typename IT>
	template <typename B, typename T> requires( IsSame< T, uint > and IsSame< B, bool >)
	SimdTInt128<IT>::SimdTInt128 (B v0, B v1, B v2, B v3) __NE___
	{
		uint  a[] = { (v0 ? ~0u : 0), (v1 ? ~0u : 0), (v2 ? ~0u : 0), (v3 ? ~0u : 0) };
		StaticAssert( CountOf(a) == 4 );
		_value = vld1q_u32( a );
	}

	template <typename IT>
	template <typename B, typename T> requires( IsSame< T, ulong > and IsSame< B, bool >)
	SimdTInt128<IT>::SimdTInt128 (B v0, B v1) __NE___
	{
		ulong  a[] = { (v0 ? ~0ull : 0), (v1 ? ~0ull : 0) };
		StaticAssert( CountOf(a) == 2 );
		_value = vld1q_u64( a );
	}

/*
=================================================
	constructor
=================================================
*/
	template <typename IT>
	template <typename T, std::enable_if_t< sizeof(T)<=4, bool >>
	SimdTInt128<IT>::SimdTInt128 (const SimdTInt64<IT> &low, const SimdTInt64<IT> &high) __NE___
	{
		if constexpr( isU8  )	_value = vcombine_u8(  low.Ref(), high.Ref() );
		if constexpr( isI8  )	_value = vcombine_s8(  low.Ref(), high.Ref() );
		if constexpr( isU16 )	_value = vcombine_u16( low.Ref(), high.Ref() );
		if constexpr( isI16 )	_value = vcombine_s16( low.Ref(), high.Ref() );
		if constexpr( isU32 )	_value = vcombine_u32( low.Ref(), high.Ref() );
		if constexpr( isI32 )	_value = vcombine_s32( low.Ref(), high.Ref() );
	}

	template <typename IT>
	template <uint Step>
	SimdTInt128<IT>::SimdTInt128 (MSBMask<count, Step> mask) __NE___
	{
		if constexpr( sizeof(IT) == 1 )
		{
			#define M(x)	sbyte(mask.template get<x>() ? -1 : 0)
			sbyte a[] = {M(0), M(1), M( 2), M( 3), M( 4), M( 5), M( 6), M( 7),
						M(8), M(9), M(10), M(11), M(12), M(13), M(14), M(15)};
			_value = vld1q_s8( a );
			#undef M
		}else
		if constexpr( sizeof(IT) == 2 )
		{
			#define M(x)	short(mask.template get<x>() ? -1 : 0)
			short a[] = {M(0), M(1), M(2), M(3), M(4), M(5), M(6), M(7)};
			_value = vld1q_s16( a );
			#undef M
		}else
		if constexpr( sizeof(IT) == 4 )
		{
			#define M(x)	int(mask.template get<x>() ? -1 : 0)
			int a[] = {M(0), M(1), M(2), M(3)};
			_value = vld1q_s32( a );
			#undef M
		}else
		if constexpr( sizeof(IT) == 8 )
		{
			#define M(x)	slong(mask.template get<x>() ? -1ll : 0)
			slong a[] = {M(0), M(1)};
			_value = vld1q_s64( a );
			#undef M
		}
	}

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
		if constexpr( isU8 )	return vgetq_lane_u8(  _value, I );
		if constexpr( isI8 )	return vgetq_lane_s8(  _value, I );
		if constexpr( isU16 )	return vgetq_lane_u16( _value, I );
		if constexpr( isI16 )	return vgetq_lane_s16( _value, I );
		if constexpr( isU32 )	return vgetq_lane_u32( _value, I );
		if constexpr( isI32 )	return vgetq_lane_s32( _value, I );
		if constexpr( isU64 )	return vgetq_lane_u64( _value, I );
		if constexpr( isI64 )	return vgetq_lane_s64( _value, I );
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
		if constexpr( isU8 )	return Self{ vsetq_lane_u8(  newValue, _value, I )};
		if constexpr( isI8 )	return Self{ vsetq_lane_s8(  newValue, _value, I )};
		if constexpr( isU16 )	return Self{ vsetq_lane_u16( newValue, _value, I )};
		if constexpr( isI16 )	return Self{ vsetq_lane_s16( newValue, _value, I )};
		if constexpr( isU32 )	return Self{ vsetq_lane_u32( newValue, _value, I )};
		if constexpr( isI32 )	return Self{ vsetq_lane_s32( newValue, _value, I )};
		if constexpr( isU64 )	return Self{ vsetq_lane_u64( newValue, _value, I )};
		if constexpr( isI64 )	return Self{ vsetq_lane_s64( newValue, _value, I )};
	}

/*
=================================================
	Add
=================================================
*/
	template <typename IT>
	SimdTInt128<IT>  SimdTInt128<IT>::Add (const Self &rhs)  C_NE___
	{
		if constexpr( isU8 )	return Self{ vaddq_u8(  _value, rhs._value )};
		if constexpr( isI8 )	return Self{ vaddq_s8(  _value, rhs._value )};
		if constexpr( isU16 )	return Self{ vaddq_u16( _value, rhs._value )};
		if constexpr( isI16 )	return Self{ vaddq_s16( _value, rhs._value )};
		if constexpr( isU32 )	return Self{ vaddq_u32( _value, rhs._value )};
		if constexpr( isI32 )	return Self{ vaddq_s32( _value, rhs._value )};
		if constexpr( isU64 )	return Self{ vaddq_u64( _value, rhs._value )};
		if constexpr( isI64 )	return Self{ vaddq_s64( _value, rhs._value )};
	}

/*
=================================================
	Sub
=================================================
*/
	template <typename IT>
	SimdTInt128<IT>  SimdTInt128<IT>::Sub (const Self &rhs)  C_NE___
	{
		if constexpr( isU8 )	return Self{ vsubq_u8(  _value, rhs._value )};
		if constexpr( isI8 )	return Self{ vsubq_s8(  _value, rhs._value )};
		if constexpr( isU16 )	return Self{ vsubq_u16( _value, rhs._value )};
		if constexpr( isI16 )	return Self{ vsubq_s16( _value, rhs._value )};
		if constexpr( isU32 )	return Self{ vsubq_u32( _value, rhs._value )};
		if constexpr( isI32 )	return Self{ vsubq_s32( _value, rhs._value )};
		if constexpr( isU64 )	return Self{ vsubq_u64( _value, rhs._value )};
		if constexpr( isI64 )	return Self{ vsubq_s64( _value, rhs._value )};
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
		if constexpr( isU8 )	return Self{ vmulq_u8(  _value, rhs._value )};
		if constexpr( isI8 )	return Self{ vmulq_s8(  _value, rhs._value )};
		if constexpr( isU16 )	return Self{ vmulq_u16( _value, rhs._value )};
		if constexpr( isI16 )	return Self{ vmulq_s16( _value, rhs._value )};
		if constexpr( isU32 )	return Self{ vmulq_u32( _value, rhs._value )};
		if constexpr( isI32 )	return Self{ vmulq_s32( _value, rhs._value )};
	}

	template <typename IT>
	SimdTInt128<IT>  SimdTInt128<IT>::Mul (Scalar_t rhs)  C_NE___
	{
		StaticAssert( Has_Mul() );
		if constexpr( isI8 or isU8 )	return Mul( Self{rhs} );
		if constexpr( isU16 )	return Self{ vmulq_n_u16( _value, rhs )};
		if constexpr( isI16 )	return Self{ vmulq_n_s16( _value, rhs )};
		if constexpr( isU32 )	return Self{ vmulq_n_u32( _value, rhs )};
		if constexpr( isI32 )	return Self{ vmulq_n_s32( _value, rhs )};
	}

	template <typename IT>
	__Ce__ bool  SimdTInt128<IT>::Has_Mul ()
	{
		return sizeof(Scalar_t) <= sizeof(uint);
	}

/*
=================================================
	AddSat
=================================================
*/
	template <typename IT>
	SimdTInt128<IT>  SimdTInt128<IT>::AddSat (const Self &rhs) C_NE___
	{
		if constexpr( isU8 )	return Self{ vqaddq_u8(  _value, rhs._value )};
		if constexpr( isI8 )	return Self{ vqaddq_s8(  _value, rhs._value )};
		if constexpr( isU16 )	return Self{ vqaddq_u16( _value, rhs._value )};
		if constexpr( isI16 )	return Self{ vqaddq_s16( _value, rhs._value )};
		if constexpr( isU32 )	return Self{ vqaddq_u32( _value, rhs._value )};
		if constexpr( isI32 )	return Self{ vqaddq_s32( _value, rhs._value )};
		if constexpr( isU64 )	return Self{ vqaddq_u64( _value, rhs._value )};
		if constexpr( isI64 )	return Self{ vqaddq_s64( _value, rhs._value )};
	}

/*
=================================================
	AddSat
=================================================
*/
# if AE_SIMD_NEON64
	template <typename IT>
	SimdTInt128<IT>  SimdTInt128<IT>::AddSat (const _InvSign_t &rhs) C_NE___
	{
		if constexpr( isU8 )	return Self{ vsqaddq_u8(  _value, rhs.Ref() )};
		if constexpr( isI8 )	return Self{ vuqaddq_s8(  _value, rhs.Ref() )};
		if constexpr( isU16 )	return Self{ vsqaddq_u16( _value, rhs.Ref() )};
		if constexpr( isI16 )	return Self{ vuqaddq_s16( _value, rhs.Ref() )};
		if constexpr( isU32 )	return Self{ vsqaddq_u32( _value, rhs.Ref() )};
		if constexpr( isI32 )	return Self{ vuqaddq_s32( _value, rhs.Ref() )};
		if constexpr( isU64 )	return Self{ vsqaddq_u64( _value, rhs.Ref() )};
		if constexpr( isI64 )	return Self{ vuqaddq_s64( _value, rhs.Ref() )};
	}
# endif
/*
=================================================
	SubSat
=================================================
*/
	template <typename IT>
	SimdTInt128<IT>  SimdTInt128<IT>::SubSat (const Self &rhs) C_NE___
	{
		if constexpr( isU8 )	return Self{ vqsubq_u8(  _value, rhs._value )};
		if constexpr( isI8 )	return Self{ vqsubq_s8(  _value, rhs._value )};
		if constexpr( isU16 )	return Self{ vqsubq_u16( _value, rhs._value )};
		if constexpr( isI16 )	return Self{ vqsubq_s16( _value, rhs._value )};
		if constexpr( isU32 )	return Self{ vqsubq_u32( _value, rhs._value )};
		if constexpr( isI32 )	return Self{ vqsubq_s32( _value, rhs._value )};
		if constexpr( isU64 )	return Self{ vqsubq_u64( _value, rhs._value )};
		if constexpr( isI64 )	return Self{ vqsubq_s64( _value, rhs._value )};
	}

/*
=================================================
	BitInverse
=================================================
*/
	template <typename IT>
	SimdTInt128<IT>  SimdTInt128<IT>::BitInverse ()  C_NE___
	{
		if constexpr( isU8 )	return Self{ vmvnq_u8(  _value )};
		if constexpr( isI8 )	return Self{ vmvnq_s8(  _value )};
		if constexpr( isU16 )	return Self{ vmvnq_u16( _value )};
		if constexpr( isI16 )	return Self{ vmvnq_s16( _value )};
		if constexpr( isU32 )	return Self{ vmvnq_u32( _value )};
		if constexpr( isI32 )	return Self{ vmvnq_s32( _value )};
		if constexpr( isU64 )	return Self{ vreinterpretq_u64_u32( vmvnq_u32( vreinterpretq_u32_u64( _value )))};
		if constexpr( isI64 )	return Self{ vreinterpretq_s64_u32( vmvnq_u32( vreinterpretq_s32_u64( _value )))};
	}

/*
=================================================
	And
=================================================
*/
	template <typename IT>
	SimdTInt128<IT>  SimdTInt128<IT>::And (const Self &rhs)  C_NE___
	{
		if constexpr( isU8 )	return Self{ vandq_u8(  _value, rhs._value )};
		if constexpr( isI8 )	return Self{ vandq_s8(  _value, rhs._value )};
		if constexpr( isU16 )	return Self{ vandq_u16( _value, rhs._value )};
		if constexpr( isI16 )	return Self{ vandq_s16( _value, rhs._value )};
		if constexpr( isU32 )	return Self{ vandq_u32( _value, rhs._value )};
		if constexpr( isI32 )	return Self{ vandq_s32( _value, rhs._value )};
		if constexpr( isU64 )	return Self{ vandq_u64( _value, rhs._value )};
		if constexpr( isI64 )	return Self{ vandq_s64( _value, rhs._value )};
	}

/*
=================================================
	Or
=================================================
*/
	template <typename IT>
	SimdTInt128<IT>  SimdTInt128<IT>::Or (const Self &rhs)  C_NE___
	{
		if constexpr( isU8 )	return Self{ vorrq_u8(  _value, rhs._value )};
		if constexpr( isI8 )	return Self{ vorrq_s8(  _value, rhs._value )};
		if constexpr( isU16 )	return Self{ vorrq_u16( _value, rhs._value )};
		if constexpr( isI16 )	return Self{ vorrq_s16( _value, rhs._value )};
		if constexpr( isU32 )	return Self{ vorrq_u32( _value, rhs._value )};
		if constexpr( isI32 )	return Self{ vorrq_s32( _value, rhs._value )};
		if constexpr( isU64 )	return Self{ vorrq_u64( _value, rhs._value )};
		if constexpr( isI64 )	return Self{ vorrq_s64( _value, rhs._value )};
	}

/*
=================================================
	Xor
=================================================
*/
	template <typename IT>
	SimdTInt128<IT>  SimdTInt128<IT>::Xor (const Self &rhs)  C_NE___
	{
		if constexpr( isU8 )	return Self{ veorq_u8(  _value, rhs._value )};
		if constexpr( isI8 )	return Self{ veorq_s8(  _value, rhs._value )};
		if constexpr( isU16 )	return Self{ veorq_u16( _value, rhs._value )};
		if constexpr( isI16 )	return Self{ veorq_s16( _value, rhs._value )};
		if constexpr( isU32 )	return Self{ veorq_u32( _value, rhs._value )};
		if constexpr( isI32 )	return Self{ veorq_s32( _value, rhs._value )};
		if constexpr( isU64 )	return Self{ veorq_u64( _value, rhs._value )};
		if constexpr( isI64 )	return Self{ veorq_s64( _value, rhs._value )};
	}

/*
=================================================
	OrNot
=================================================
*/
	template <typename IT>
	SimdTInt128<IT>  SimdTInt128<IT>::OrNot (const Self &rhs)  C_NE___
	{
		if constexpr( isU8 )	return Self{ vornq_u8(  _value, rhs._value )};
		if constexpr( isI8 )	return Self{ vornq_s8(  _value, rhs._value )};
		if constexpr( isU16 )	return Self{ vornq_u16( _value, rhs._value )};
		if constexpr( isI16 )	return Self{ vornq_s16( _value, rhs._value )};
		if constexpr( isU32 )	return Self{ vornq_u32( _value, rhs._value )};
		if constexpr( isI32 )	return Self{ vornq_s32( _value, rhs._value )};
		if constexpr( isU64 )	return Self{ vornq_u64( _value, rhs._value )};
		if constexpr( isI64 )	return Self{ vornq_s64( _value, rhs._value )};
	}

/*
=================================================
	AndNot
=================================================
*/
	template <typename IT>
	SimdTInt128<IT>  SimdTInt128<IT>::AndNot (const Self &rhs)  C_NE___
	{
		if constexpr( isU8 )	return Self{ vbicq_u8(  rhs._value, _value )};
		if constexpr( isI8 )	return Self{ vbicq_s8(  rhs._value, _value )};
		if constexpr( isU16 )	return Self{ vbicq_u16( rhs._value, _value )};
		if constexpr( isI16 )	return Self{ vbicq_s16( rhs._value, _value )};
		if constexpr( isU32 )	return Self{ vbicq_u32( rhs._value, _value )};
		if constexpr( isI32 )	return Self{ vbicq_s32( rhs._value, _value )};
		if constexpr( isU64 )	return Self{ vbicq_u64( rhs._value, _value )};
		if constexpr( isI64 )	return Self{ vbicq_s64( rhs._value, _value )};
	}

/*
=================================================
	AllBits
=================================================
*/
	template <typename IT>
	SimdTInt128<IT>  SimdTInt128<IT>::AllBits (const Self &mask) C_NE___
	{
		if constexpr( isU8 )	return Self{ vtstq_u8(  _value, mask._value )};
		if constexpr( isI8 )	return Self{ vtstq_s8(  _value, mask._value )};
		if constexpr( isU16 )	return Self{ vtstq_u16( _value, mask._value )};
		if constexpr( isI16 )	return Self{ vtstq_s16( _value, mask._value )};
		if constexpr( isU32 )	return Self{ vtstq_u32( _value, mask._value )};
		if constexpr( isI32 )	return Self{ vtstq_s32( _value, mask._value )};
	  #if AE_SIMD_NEON64
		if constexpr( isU64 )	return Self{ vtstq_u64( _value, mask._value )};
		if constexpr( isI64 )	return Self{ vtstq_s64( _value, mask._value )};
	  #endif
	}

#if AE_SIMD_NEON64
/*
=================================================
	ReduceAddScalar
=================================================
*/
	template <typename IT>
	IT  SimdTInt128<IT>::ReduceAddScalar () C_NE___
	{
		StaticAssert( Has_ReduceAdd() );
		if constexpr( isU8 )	return vaddvq_u8(  _value );
		if constexpr( isI8 )	return vaddvq_s8(  _value );
		if constexpr( isU16 )	return vaddvq_u16( _value );
		if constexpr( isI16 )	return vaddvq_s16( _value );
		if constexpr( isU32 )	return vaddvq_u32( _value );
		if constexpr( isI32 )	return vaddvq_s32( _value );
		if constexpr( isU64 )	return vaddvq_u64( _value );
		if constexpr( isI64 )	return vaddvq_s64( _value );
	}

/*
=================================================
	ReduceAddExtScalar
=================================================
*/
	template <typename IT>
	auto  SimdTInt128<IT>::ReduceAddExtScalar () C_NE___
	{
		StaticAssert( Has_ReduceAddExt() );
		if constexpr( isU8 )	return ushort{vaddlvq_u8(  _value )};
		if constexpr( isI8 )	return sshort{vaddlvq_s8(  _value )};
		if constexpr( isU16 )	return uint{  vaddlvq_u16( _value )};
		if constexpr( isI16 )	return sint{  vaddlvq_s16( _value )};
		if constexpr( isU32 )	return ulong{ vaddlvq_u32( _value )};
		if constexpr( isI32 )	return slong{ vaddlvq_s32( _value )};
	}

	template <typename IT>
	__Ce__ bool  SimdTInt128<IT>::Has_ReduceAddExt ()
	{
		return sizeof(Scalar_t) <= sizeof(uint);
	}

/*
=================================================
	ReduceMaxScalar
=================================================
*/
	template <typename IT>
	IT  SimdTInt128<IT>::ReduceMaxScalar () C_NE___
	{
		StaticAssert( Has_ReduceMinMax() );
		if constexpr( isU8 )	return vmaxvq_u8(  _value );
		if constexpr( isI8 )	return vmaxvq_s8(  _value );
		if constexpr( isU16 )	return vmaxvq_u16( _value );
		if constexpr( isI16 )	return vmaxvq_s16( _value );
		if constexpr( isU32 )	return vmaxvq_u32( _value );
		if constexpr( isI32 )	return vmaxvq_s32( _value );
	}

	template <typename IT>
	__Ce__ bool  SimdTInt128<IT>::Has_ReduceMinMax ()
	{
		return sizeof(Scalar_t) <= sizeof(uint);
	}

/*
=================================================
	ReduceMinScalar
=================================================
*/
	template <typename IT>
	IT  SimdTInt128<IT>::ReduceMinScalar () C_NE___
	{
		StaticAssert( Has_ReduceMinMax() );
		if constexpr( isU8 )	return vminvq_u8(  _value );
		if constexpr( isI8 )	return vminvq_s8(  _value );
		if constexpr( isU16 )	return vminvq_u16( _value );
		if constexpr( isI16 )	return vminvq_s16( _value );
		if constexpr( isU32 )	return vminvq_u32( _value );
		if constexpr( isI32 )	return vminvq_s32( _value );
	}

#endif // AE_SIMD_NEON64

/*
=================================================
	_LShift_LogicV
=================================================
*/
	template <typename IT>
	SimdTInt128<IT>  SimdTInt128<IT>::_LShift_LogicV (const Signed_t &shift)  C_NE___
	{
		StaticAssert( Has_VecLShift_Logic() );
		if constexpr( Has_Greater() ) {
			ASSERT_MSG( shift.Abs().LEqual( Signed_t{CT_SizeOfInBits<Scalar_t>} ).All(), "Result will be zero" );
		}
		if constexpr( isI8 )	return Self{ vshlq_s8(  _value, shift.Ref() )};
		if constexpr( isU8 )	return Self{ vshlq_u8(  _value, shift.Ref() )};
		if constexpr( isI16 )	return Self{ vshlq_s16( _value, shift.Ref() )};
		if constexpr( isU16 )	return Self{ vshlq_u16( _value, shift.Ref() )};
		if constexpr( isI32 )	return Self{ vshlq_s32( _value, shift.Ref() )};
		if constexpr( isU32 )	return Self{ vshlq_u32( _value, shift.Ref() )};
		if constexpr( isI64 )	return Self{ vshlq_s64( _value, shift.Ref() )};
		if constexpr( isU64 )	return Self{ vshlq_u64( _value, shift.Ref() )};
	}

/*
=================================================
	_LShift_ArithV
=================================================
*/
	template <typename IT>
	SimdTInt128<IT>  SimdTInt128<IT>::_LShift_ArithV (const Signed_t &shift)  C_NE___
	{
		StaticAssert( Has_VecLShift_Arithmetic() );
		if constexpr( Has_Greater() ) {
			ASSERT_MSG( shift.Abs().LEqual( Signed_t{CT_SizeOfInBits<Scalar_t>} ).All(), "Result will be zero" );
		}
		if constexpr( isI8 )	return Self{ vqshlq_s8(  _value, shift.Ref() )};
		if constexpr( isU8 )	return Self{ vqshlq_u8(  _value, shift.Ref() )};
		if constexpr( isI16 )	return Self{ vqshlq_s16( _value, shift.Ref() )};
		if constexpr( isU16 )	return Self{ vqshlq_u16( _value, shift.Ref() )};
		if constexpr( isI32 )	return Self{ vqshlq_s32( _value, shift.Ref() )};
		if constexpr( isU32 )	return Self{ vqshlq_u32( _value, shift.Ref() )};
		if constexpr( isI64 )	return Self{ vqshlq_s64( _value, shift.Ref() )};
		if constexpr( isU64 )	return Self{ vqshlq_u64( _value, shift.Ref() )};
	}

/*
=================================================
	LShift_Logic
----
	returns zero element on overflow
=================================================
*/
	template <typename IT>
	template <uint Shift>
	SimdTInt128<IT>  SimdTInt128<IT>::LShift_Logic ()  C_NE___
	{
		StaticAssert( Has_ScalarShift_Logic() );
		StaticAssert( Shift <= CT_SizeOfInBits<Scalar_t> );

		if constexpr( isI8 )	return Self{ vshlq_n_s8(  _value, Shift )};
		if constexpr( isU8 )	return Self{ vshlq_n_u8(  _value, Shift )};
		if constexpr( isI16 )	return Self{ vshlq_n_s16( _value, Shift )};
		if constexpr( isU16 )	return Self{ vshlq_n_u16( _value, Shift )};
		if constexpr( isI32 )	return Self{ vshlq_n_s32( _value, Shift )};
		if constexpr( isU32 )	return Self{ vshlq_n_u32( _value, Shift )};
		if constexpr( isI64 )	return Self{ vshlq_n_s64( _value, Shift )};
		if constexpr( isU64 )	return Self{ vshlq_n_u64( _value, Shift )};
	}

/*
=================================================
	LShift_Arith
----
	returns max element on unsigned overflow
	returns -1 element on signed overflow
=================================================
*/
	template <typename IT>
	template <uint Shift>
	SimdTInt128<IT>  SimdTInt128<IT>::LShift_Arith ()  C_NE___
	{
		StaticAssert( Has_ScalarShift_Arithmetic() );
		StaticAssert( Shift <= CT_SizeOfInBits<Scalar_t> );

		if constexpr( isI8 )	return Self{ vqshlq_n_s8(  _value, Shift )};
		if constexpr( isU8 )	return Self{ vqshlq_n_u8(  _value, Shift )};
		if constexpr( isI16 )	return Self{ vqshlq_n_s16( _value, Shift )};
		if constexpr( isU16 )	return Self{ vqshlq_n_u16( _value, Shift )};
		if constexpr( isI32 )	return Self{ vqshlq_n_s32( _value, Shift )};
		if constexpr( isU32 )	return Self{ vqshlq_n_u32( _value, Shift )};
		if constexpr( isI64 )	return Self{ vqshlq_n_s64( _value, Shift )};
		if constexpr( isU64 )	return Self{ vqshlq_n_u64( _value, Shift )};
	}

/*
=================================================
	RShift_Logic
----
	returns zero element on overflow
=================================================
*/
	template <typename IT>
	template <uint Shift>
	SimdTInt128<IT>  SimdTInt128<IT>::RShift_Logic ()  C_NE___
	{
		StaticAssert( Has_ScalarShift_Logic() );
		StaticAssert( Shift > 0 );
		StaticAssert( Shift <= CT_SizeOfInBits<Scalar_t> );

		if constexpr( isI8 )	return Self{ vshrq_n_s8(  _value, Shift )};
		if constexpr( isU8 )	return Self{ vshrq_n_u8(  _value, Shift )};
		if constexpr( isI16 )	return Self{ vshrq_n_s16( _value, Shift )};
		if constexpr( isU16 )	return Self{ vshrq_n_u16( _value, Shift )};
		if constexpr( isI32 )	return Self{ vshrq_n_s32( _value, Shift )};
		if constexpr( isU32 )	return Self{ vshrq_n_u32( _value, Shift )};
		if constexpr( isI64 )	return Self{ vshrq_n_s64( _value, Shift )};
		if constexpr( isU64 )	return Self{ vshrq_n_u64( _value, Shift )};
	}

/*
=================================================
	RShift_Arith
----
	returns max element on unsigned overflow
	returns -1 element on signed overflow
=================================================
*/
	template <typename IT>
	template <uint Shift>
	SimdTInt128<IT>  SimdTInt128<IT>::RShift_Arith ()  C_NE___
	{
		StaticAssert( Has_ScalarShift_Arithmetic() );
		StaticAssert( Shift <= CT_SizeOfInBits<Scalar_t> );
		return RShift_Arith( Shift );
	}

/*
=================================================
	Min / Max
=================================================
*/
	template <typename IT>
	SimdTInt128<IT>  SimdTInt128<IT>::Min (const Self &rhs)  C_NE___
	{
		StaticAssert( Has_MinMax() );
		if constexpr( isU8 )	return Self{ vminq_u8(  _value, rhs._value )};
		if constexpr( isI8 )	return Self{ vminq_s8(  _value, rhs._value )};
		if constexpr( isU16 )	return Self{ vminq_u16( _value, rhs._value )};
		if constexpr( isI16 )	return Self{ vminq_s16( _value, rhs._value )};
		if constexpr( isU32 )	return Self{ vminq_u32( _value, rhs._value )};
		if constexpr( isI32 )	return Self{ vminq_s32( _value, rhs._value )};
	}

	template <typename IT>
	SimdTInt128<IT>  SimdTInt128<IT>::Max (const Self &rhs)  C_NE___
	{
		StaticAssert( Has_MinMax() );
		if constexpr( isU8 )	return Self{ vmaxq_u8(  _value, rhs._value )};
		if constexpr( isI8 )	return Self{ vmaxq_s8(  _value, rhs._value )};
		if constexpr( isU16 )	return Self{ vmaxq_u16( _value, rhs._value )};
		if constexpr( isI16 )	return Self{ vmaxq_s16( _value, rhs._value )};
		if constexpr( isU32 )	return Self{ vmaxq_u32( _value, rhs._value )};
		if constexpr( isI32 )	return Self{ vmaxq_s32( _value, rhs._value )};
	}

	template <typename IT>
	__Ce__ bool  SimdTInt128<IT>::Has_MinMax ()
	{
		return sizeof(Scalar_t) <= sizeof(uint);
	}

/*
=================================================
	IsZero
=================================================
*/
	template <typename IT>
	typename SimdTInt128<IT>::Bool_t  SimdTInt128<IT>::IsZero () C_NE___
	{
	  #if AE_SIMD_NEON64
		if constexpr( isU8 )	return Bool_t{ vceqzq_u8(  _value )};
		if constexpr( isI8 )	return Bool_t{ vceqzq_s8(  _value )};
		if constexpr( isU16 )	return Bool_t{ vceqzq_u16( _value )};
		if constexpr( isI16 )	return Bool_t{ vceqzq_s16( _value )};
		if constexpr( isU32 )	return Bool_t{ vceqzq_u32( _value )};
		if constexpr( isI32 )	return Bool_t{ vceqzq_s32( _value )};
		if constexpr( isU64 )	return Bool_t{ vceqzq_u64( _value )};
		if constexpr( isI64 )	return Bool_t{ vceqzq_s64( _value )};
	  #else
		return Equal( Self{} );
	  #endif
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
		if constexpr( isU8 )	return Bool_t{ vceqq_u8(  _value, rhs._value )};
		if constexpr( isI8 )	return Bool_t{ vceqq_s8(  _value, rhs._value )};
		if constexpr( isU16 )	return Bool_t{ vceqq_u16( _value, rhs._value )};
		if constexpr( isI16 )	return Bool_t{ vceqq_s16( _value, rhs._value )};
		if constexpr( isU32 )	return Bool_t{ vceqq_u32( _value, rhs._value )};
		if constexpr( isI32 )	return Bool_t{ vceqq_s32( _value, rhs._value )};
	  #if AE_SIMD_NEON64
		if constexpr( isU64 )	return Bool_t{ vceqq_u64( _value, rhs._value )};
		if constexpr( isI64 )	return Bool_t{ vceqq_s64( _value, rhs._value )};
	  #endif
	}

/*
=================================================
	Greater
=================================================
*/
	template <typename IT>
	typename SimdTInt128<IT>::Bool_t  SimdTInt128<IT>::Greater (const Self &rhs)  C_NE___
	{
		StaticAssert( Has_Greater() );
		if constexpr( isU8 )	return Bool_t{ vcgtq_u8(  _value, rhs._value )};
		if constexpr( isI8 )	return Bool_t{ vcgtq_s8(  _value, rhs._value )};
		if constexpr( isU16 )	return Bool_t{ vcgtq_u16( _value, rhs._value )};
		if constexpr( isI16 )	return Bool_t{ vcgtq_s16( _value, rhs._value )};
		if constexpr( isU32 )	return Bool_t{ vcgtq_u32( _value, rhs._value )};
		if constexpr( isI32 )	return Bool_t{ vcgtq_s32( _value, rhs._value )};
	  #if AE_SIMD_NEON64
		if constexpr( isU64 )	return Bool_t{ vcgtq_u64( _value, rhs._value )};
		if constexpr( isI64 )	return Bool_t{ vcgtq_s64( _value, rhs._value )};
	  #endif
	}

/*
=================================================
	Less
=================================================
*/
	template <typename IT>
	typename SimdTInt128<IT>::Bool_t  SimdTInt128<IT>::Less (const Self &rhs)  C_NE___
	{
		StaticAssert( Has_Greater() );
		if constexpr( isU8 )	return Bool_t{ vcltq_u8(  _value, rhs._value )};
		if constexpr( isI8 )	return Bool_t{ vcltq_s8(  _value, rhs._value )};
		if constexpr( isU16 )	return Bool_t{ vcltq_u16( _value, rhs._value )};
		if constexpr( isI16 )	return Bool_t{ vcltq_s16( _value, rhs._value )};
		if constexpr( isU32 )	return Bool_t{ vcltq_u32( _value, rhs._value )};
		if constexpr( isI32 )	return Bool_t{ vcltq_s32( _value, rhs._value )};
	  #if AE_SIMD_NEON64
		if constexpr( isU64 )	return Bool_t{ vcltq_u64( _value, rhs._value )};
		if constexpr( isI64 )	return Bool_t{ vcltq_s64( _value, rhs._value )};
	  #endif
	}

/*
=================================================
	LEqual
=================================================
*/
	template <typename IT>
	typename SimdTInt128<IT>::Bool_t  SimdTInt128<IT>::LEqual (const Self &rhs)  C_NE___
	{
		StaticAssert( Has_Greater() );
		if constexpr( isU8 )	return Bool_t{ vcleq_u8(  _value, rhs._value )};
		if constexpr( isI8 )	return Bool_t{ vcleq_s8(  _value, rhs._value )};
		if constexpr( isU16 )	return Bool_t{ vcleq_u16( _value, rhs._value )};
		if constexpr( isI16 )	return Bool_t{ vcleq_s16( _value, rhs._value )};
		if constexpr( isU32 )	return Bool_t{ vcleq_u32( _value, rhs._value )};
		if constexpr( isI32 )	return Bool_t{ vcleq_s32( _value, rhs._value )};
	  #if AE_SIMD_NEON64
		if constexpr( isU64 )	return Bool_t{ vcleq_u64( _value, rhs._value )};
		if constexpr( isI64 )	return Bool_t{ vcleq_s64( _value, rhs._value )};
	  #endif
	}

/*
=================================================
	GEqual
=================================================
*/
	template <typename IT>
	typename SimdTInt128<IT>::Bool_t  SimdTInt128<IT>::GEqual (const Self &rhs)  C_NE___
	{
		StaticAssert( Has_Greater() );
		if constexpr( isU8 )	return Bool_t{ vcgeq_u8(  _value, rhs._value )};
		if constexpr( isI8 )	return Bool_t{ vcgeq_s8(  _value, rhs._value )};
		if constexpr( isU16 )	return Bool_t{ vcgeq_u16( _value, rhs._value )};
		if constexpr( isI16 )	return Bool_t{ vcgeq_s16( _value, rhs._value )};
		if constexpr( isU32 )	return Bool_t{ vcgeq_u32( _value, rhs._value )};
		if constexpr( isI32 )	return Bool_t{ vcgeq_s32( _value, rhs._value )};
	  #if AE_SIMD_NEON64
		if constexpr( isU64 )	return Bool_t{ vcgeq_u64( _value, rhs._value )};
		if constexpr( isI64 )	return Bool_t{ vcgeq_s64( _value, rhs._value )};
	  #endif
	}

/*
=================================================
	Abs
=================================================
*/
	template <typename IT>
	template <typename T> requires( IsSignedInteger<T> )
	SimdTInt128<IT>  SimdTInt128<IT>::Abs ()  C_NE___
	{
		if constexpr( isI8 )	return Self{ vabsq_s8( _value )};
		if constexpr( isI16 )	return Self{ vabsq_s16( _value )};
		if constexpr( isI32 )	return Self{ vabsq_s32( _value )};
	  #if AE_SIMD_NEON64
		if constexpr( isI64 )	return Self{ vabsq_s64( _value )};
	  #endif
	}

/*
=================================================
	Negative
=================================================
*/
	template <typename IT>
	template <typename T> requires( IsSignedInteger<T> )
	SimdTInt128<IT>  SimdTInt128<IT>::Negative ()  C_NE___
	{
		if constexpr( isI8 )	return Self{ vnegq_s8(  _value )};
		if constexpr( isI16 )	return Self{ vnegq_s16( _value )};
		if constexpr( isI32 )	return Self{ vnegq_s32( _value )};
	  #if AE_SIMD_NEON64
		if constexpr( isI64 )	return Self{ vnegq_s64( _value )};
	  #else
		if constexpr( isI64 )	return Self{slong{0}}.Sub( *this );
	  #endif
	}

/*
=================================================
	NegativeSat
=================================================
*/
	template <typename IT>
	template <typename T> requires( IsSignedInteger<T> )
	SimdTInt128<IT>  SimdTInt128<IT>::NegativeSat ()  C_NE___
	{
		if constexpr( isI8 )	return Self{ vqnegq_s8(  _value )};
		if constexpr( isI16 )	return Self{ vqnegq_s16( _value )};
		if constexpr( isI32 )	return Self{ vqnegq_s32( _value )};
	  #if AE_SIMD_NEON64
		if constexpr( isI64 )	return Self{ vqnegq_s64( _value )};
	  #endif
	}

/*
=================================================
	Part
=================================================
*/
	template <typename IT>
	template <uint Idx>
	auto  SimdTInt128<IT>::Part () C_NE___
	{
		if constexpr( parts == 2 ){
			StaticAssert( Idx < parts );
			if constexpr( Idx == 0 ){
				if constexpr( isI8 )	return SimdTInt64<IT>{ vget_low_s8(  _value )};
				if constexpr( isU8 )	return SimdTInt64<IT>{ vget_low_u8(  _value )};
				if constexpr( isI16 )	return SimdTInt64<IT>{ vget_low_s16( _value )};
				if constexpr( isU16 )	return SimdTInt64<IT>{ vget_low_u16( _value )};
				if constexpr( isI32 )	return SimdTInt64<IT>{ vget_low_s32( _value )};
				if constexpr( isU32 )	return SimdTInt64<IT>{ vget_low_u32( _value )};
			}else{
				if constexpr( isI8 )	return SimdTInt64<IT>{ vget_high_s8(  _value )};
				if constexpr( isU8 )	return SimdTInt64<IT>{ vget_high_u8(  _value )};
				if constexpr( isI16 )	return SimdTInt64<IT>{ vget_high_s16( _value )};
				if constexpr( isU16 )	return SimdTInt64<IT>{ vget_high_u16( _value )};
				if constexpr( isI32 )	return SimdTInt64<IT>{ vget_high_s32( _value )};
				if constexpr( isU32 )	return SimdTInt64<IT>{ vget_high_u32( _value )};
			}
		}else
		if constexpr( parts == 1 ){
			StaticAssert( Idx == 0 );
			return *this;
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
		return Self{ __builtin_shufflevector( _value, _value, X, Y, Z, W )};
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
		return Self{ __builtin_shufflevector( _value, _value, X, Y )};
	}

/*
=================================================
	MulAdd
=================================================
*/
	template <typename IT>
	ND_ SimdTInt128<IT>  MulAdd (const SimdTInt128<IT> &a, const SimdTInt128<IT> &b, const SimdTInt128<IT> &c) __NE___
	{
		StaticAssert( SimdTInt128<IT>::Has_MulAdd() );
		if constexpr( IsSame< IT, sshort >)	return SimdTInt128<IT>{ vmlaq_s16( c._value, a._value, b._value )};
		if constexpr( IsSame< IT, ushort >)	return SimdTInt128<IT>{ vmlaq_u16( c._value, a._value, b._value )};
		if constexpr( IsSame< IT, sint   >)	return SimdTInt128<IT>{ vmlaq_s32( c._value, a._value, b._value )};
		if constexpr( IsSame< IT, uint   >)	return SimdTInt128<IT>{ vmlaq_u32( c._value, a._value, b._value )};
	}

	template <typename IT>
	ND_ SimdTInt128<IT>  MulAdd (const SimdTInt128<IT> &a, const IT b, const SimdTInt128<IT> &c) __NE___
	{
		StaticAssert( SimdTInt128<IT>::Has_MulAdd() );
		if constexpr( IsSame< IT, sshort >)	return SimdTInt128<IT>{ vmlaq_n_s16( c._value, a._value, b )};
		if constexpr( IsSame< IT, ushort >)	return SimdTInt128<IT>{ vmlaq_n_u16( c._value, a._value, b )};
		if constexpr( IsSame< IT, sint   >)	return SimdTInt128<IT>{ vmlaq_n_s32( c._value, a._value, b )};
		if constexpr( IsSame< IT, uint   >)	return SimdTInt128<IT>{ vmlaq_n_u32( c._value, a._value, b )};
	}

/*
=================================================
	NegMulAdd
=================================================
*/
	template <typename IT>
	ND_ SimdTInt128<IT>  NegMulAdd (const SimdTInt128<IT> &a, const SimdTInt128<IT> &b, const SimdTInt128<IT> &c) __NE___
	{
		StaticAssert( SimdTInt128<IT>::Has_MulAdd() );
		if constexpr( IsSame< IT, sbyte  >)	return SimdTInt128<IT>{ vmlsq_s8(  c._value, a._value, b._value )};
		if constexpr( IsSame< IT, ubyte  >)	return SimdTInt128<IT>{ vmlsq_u8(  c._value, a._value, b._value )};
		if constexpr( IsSame< IT, sshort >)	return SimdTInt128<IT>{ vmlsq_s16( c._value, a._value, b._value )};
		if constexpr( IsSame< IT, ushort >)	return SimdTInt128<IT>{ vmlsq_u16( c._value, a._value, b._value )};
		if constexpr( IsSame< IT, sint   >)	return SimdTInt128<IT>{ vmlsq_s32( c._value, a._value, b._value )};
		if constexpr( IsSame< IT, uint   >)	return SimdTInt128<IT>{ vmlsq_u32( c._value, a._value, b._value )};
	}

	template <typename IT>
	ND_ SimdTInt128<IT>  NegMulAdd (const SimdTInt128<IT> &a, const IT b, const SimdTInt128<IT> &c) __NE___
	{
		StaticAssert( SimdTInt128<IT>::Has_MulAdd() );
		if constexpr( IsSame< IT, sshort >)	return SimdTInt128<IT>{ vmlsq_n_s16( c._value, a._value, b )};
		if constexpr( IsSame< IT, ushort >)	return SimdTInt128<IT>{ vmlsq_n_u16( c._value, a._value, b )};
		if constexpr( IsSame< IT, sint   >)	return SimdTInt128<IT>{ vmlsq_n_s32( c._value, a._value, b )};
		if constexpr( IsSame< IT, uint   >)	return SimdTInt128<IT>{ vmlsq_n_u32( c._value, a._value, b )};
	}

/*
=================================================
	Select
=================================================
*/
	template <typename IT>
	SimdTInt128<IT>  SimdTInt128<IT>::_Select (const typename SimdTInt128<IT>::Bool_t &condition, const SimdTInt128<IT> &ifTrue, const SimdTInt128<IT> &ifFalse) __NE___
	{
		if constexpr( IsSame< IT, sbyte  >)	return SimdTInt128<IT>{ vbslq_s8(  condition.Ref(), ifTrue._value, ifFalse._value )};
		if constexpr( IsSame< IT, ubyte  >)	return SimdTInt128<IT>{ vbslq_u8(  condition.Ref(), ifTrue._value, ifFalse._value )};
		if constexpr( IsSame< IT, sshort >)	return SimdTInt128<IT>{ vbslq_s16( condition.Ref(), ifTrue._value, ifFalse._value )};
		if constexpr( IsSame< IT, ushort >)	return SimdTInt128<IT>{ vbslq_u16( condition.Ref(), ifTrue._value, ifFalse._value )};
		if constexpr( IsSame< IT, sint   >)	return SimdTInt128<IT>{ vbslq_s32( condition.Ref(), ifTrue._value, ifFalse._value )};
		if constexpr( IsSame< IT, uint   >)	return SimdTInt128<IT>{ vbslq_u32( condition.Ref(), ifTrue._value, ifFalse._value )};
		if constexpr( IsSame< IT, slong  >)	return SimdTInt128<IT>{ vbslq_s64( condition.Ref(), ifTrue._value, ifFalse._value )};
		if constexpr( IsSame< IT, ulong  >)	return SimdTInt128<IT>{ vbslq_u64( condition.Ref(), ifTrue._value, ifFalse._value )};
	}

/*
=================================================
	ToBitfield
=================================================
*/
	template <typename IT>
	template <typename T> requires( IsUnsignedInteger<T> )
	typename SimdTInt128<IT>::Mask_t  SimdTInt128<IT>::ToBitfield ()  C_NE___
	{
		if constexpr( isU8 )
		{
			const sbyte		shift[]  = {0,1,2,3,4,5,6,7, 0,1,2,3,4,5,6,7};
			auto	a  = vshrq_n_u8( _value, 7 );
			auto	b  = vshlq_u8( a, vld1q_s8(shift) );
			auto	c0 = vmovl_u8( vget_low_u8( b ));
			auto	c1 = vmovl_u8( vget_high_u8( b ));
		  #if AE_SIMD_NEON64
			return Mask_t{ vaddvq_u16( c0 ) | (vaddvq_u16( c1 ) << 8) };
		  #else
			uint	v[4], w[4];
			vst1q_u32( OUT v, vreinterpretq_u32_u16( c0 ));
			vst1q_u32( OUT w, vreinterpretq_u32_u16( c1 ));
			uint	u = (v[0] | v[1] | v[2] | v[3]) | ((w[0] | w[1] | w[2] | w[3]) << 8);
			return Mask_t{ (u | (u >> 16)) & 0xFFFF };
		  #endif
		}
		if constexpr( isU16 )
		{
			const sshort	shift[] = {0,1,2,3,4,5,6,7};
			auto	a = vshrq_n_u16( _value, 15 );
			auto	b = vshlq_u16( a, vld1q_s16(shift) );
		  #if AE_SIMD_NEON64
			return Mask_t{ vaddvq_u16( b )};
		  #else
			uint	v[4];
			vst1q_u32( OUT v, vreinterpretq_u32_u16( b ));
			uint	u = v[0] | v[1] | v[2] | v[3];
			return Mask_t{ (u | (u >> 16)) & 0xFFFF };
		  #endif
		}
		if constexpr( isU32 )
		{
			const sint		shift[] = {0,1,2,3};
			auto	a = vshrq_n_u32( _value, 31 );
			auto	b = vshlq_u32( a, vld1q_s32(shift) );
		  #if AE_SIMD_NEON64
			return Mask_t{ vaddvq_u32( b )};
		  #else
			uint	v[4];
			vst1q_u32( OUT v, b );
			return Mask_t{ v[0] | v[1] | v[2] | v[3] };
		  #endif
		}
		if constexpr( isU64 )
		{
			const slong		shift[] = {0,1};
			auto	a = vshrq_n_u64( _value, 63 );
			auto	b = vshlq_u64( a, vld1q_s64(shift) );
		  #if AE_SIMD_NEON64
			return Mask_t{uint(vaddvq_u64( b ))};
		  #else
			ulong	v[2];
			vst1q_u64( OUT v, b );
			return Mask_t{uint( v[0] | v[1] )};
		  #endif
		}
	}

/*
=================================================
	All
=================================================
*/
	template <typename IT>
	template <typename T> requires( IsUnsignedInteger<T> )
	bool  SimdTInt128<IT>::All ()  C_NE___
	{
	#if AE_SIMD_NEON64
		if constexpr( isU8  )	return vaddvq_u8(  vshrq_n_u8(  _value, 4 )) == (0xFF >> 4)*16;
		if constexpr( isU16 )	return vaddvq_u16( vshrq_n_u16( _value, 3 )) == (0xFFFF >> 3)*8;
		if constexpr( isU32 )	return vaddvq_u32( vshrq_n_u32( _value, 2 )) == (0xFFFF'FFFF >> 2)*4;
		if constexpr( isU64 )	return vaddvq_u64( vshrq_n_u64( _value, 1 )) == (~ulong(0) >> 1)*2;
	#else
		uint64x2_t	a;
		if constexpr( isU8  )	a = vreinterpretq_u64_u8(  _value );
		if constexpr( isU16 )	a = vreinterpretq_u64_u16( _value );
		if constexpr( isU32 )	a = vreinterpretq_u64_u32( _value );
		if constexpr( isU64 )	a = _value;
		return	vgetq_lane_u64( a, 0 ) == UMax and
				vgetq_lane_u64( a, 1 ) == UMax;
	#endif
	}

/*
=================================================
	Any
=================================================
*/
	template <typename IT>
	template <typename T> requires( IsUnsignedInteger<T> )
	bool  SimdTInt128<IT>::Any ()  C_NE___
	{
	#if AE_SIMD_NEON64
		if constexpr( isU8  )	return vaddvq_u8(  vshrq_n_u8(  _value, 4 )) != 0;
		if constexpr( isU16 )	return vaddvq_u16( vshrq_n_u16( _value, 3 )) != 0;
		if constexpr( isU32 )	return vaddvq_u32( vshrq_n_u32( _value, 2 )) != 0;
		if constexpr( isU64 )	return vaddvq_u64( vshrq_n_u64( _value, 1 )) != 0;
	#else
		uint64x2_t	a;
		if constexpr( isU8  )	a = vreinterpretq_u64_u8(  _value );
		if constexpr( isU16 )	a = vreinterpretq_u64_u16( _value );
		if constexpr( isU32 )	a = vreinterpretq_u64_u32( _value );
		if constexpr( isU64 )	a = _value;
		return (vgetq_lane_u64( a, 0 ) | vgetq_lane_u64( a, 1 )) != 0;
	#endif
	}

/*
=================================================
	None
=================================================
*/
	template <typename IT>
	template <typename T> requires( IsUnsignedInteger<T> )
	bool  SimdTInt128<IT>::None ()  C_NE___
	{
	#if AE_SIMD_NEON64
		if constexpr( isU8  )	return vaddvq_u8(  vshrq_n_u8(  _value, 4 )) == 0;
		if constexpr( isU16 )	return vaddvq_u16( vshrq_n_u16( _value, 3 )) == 0;
		if constexpr( isU32 )	return vaddvq_u32( vshrq_n_u32( _value, 2 )) == 0;
		if constexpr( isU64 )	return vaddvq_u64( vshrq_n_u64( _value, 1 )) == 0;
	#else
		uint64x2_t	a;
		if constexpr( isU8  )	a = vreinterpretq_u64_u8(  _value );
		if constexpr( isU16 )	a = vreinterpretq_u64_u16( _value );
		if constexpr( isU32 )	a = vreinterpretq_u64_u32( _value );
		if constexpr( isU64 )	a = _value;
		return (vgetq_lane_u64( a, 0 ) | vgetq_lane_u64( a, 1 )) == 0;
	#endif
	}

/*
=================================================
	ToArray
=================================================
*/
	template <typename IT>
	void  SimdTInt128<IT>::ToArray (OUT Scalar_t* dst)  C_NE___
	{
		NonNull( dst );
		if constexpr( isI8 )	vst1q_s8(  OUT dst, _value );
		if constexpr( isU8 )	vst1q_u8(  OUT dst, _value );
		if constexpr( isI16 )	vst1q_s16( OUT dst, _value );
		if constexpr( isU16 )	vst1q_u16( OUT dst, _value );
		if constexpr( isI32 )	vst1q_s32( OUT dst, _value );
		if constexpr( isU32 )	vst1q_u32( OUT dst, _value );
		if constexpr( isI64 )	vst1q_s64( OUT dst, _value );
		if constexpr( isU64 )	vst1q_u64( OUT dst, _value );
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
	ToHalf
=================================================
*/
# ifdef AE_SIMD_SimdHalf8
	template <typename IT>
	template <uint Idx>
	auto  SimdTInt128<IT>::ToHalf () C_NE___
	{
		if constexpr( isI16 ){
			StaticAssert( Idx == 0 );
			return SimdHalf8{ vcvtq_f16_s16( _value )};
		}else
		if constexpr( isU16 ){
			StaticAssert( Idx == 0 );
			return SimdHalf8{ vcvtq_f16_u16( _value )};
		}else
		if constexpr( sizeof(IT) >= sizeof(uint) ){
			StaticAssert( Idx == 0 );
			return ToFloat().ToHalf();
		}else{
			return ToShort<Idx>().ToHalf();
		}
		// TODO: 32bit to Half4
	}
# endif
/*
=================================================
	ToFloat
=================================================
*/
# ifdef AE_SIMD_SimdFloat4
	template <typename IT>
	template <uint Idx>
	auto  SimdTInt128<IT>::ToFloat () C_NE___
	{
		if constexpr( isI32 ){
			StaticAssert( Idx == 0 );
			return SimdFloat4{ vcvtq_f32_s32( _value )};
		}else
		if constexpr( isU32 ){
			StaticAssert( Idx == 0 );
			return SimdFloat4{ vcvtq_f32_u32( _value )};
		}else
	  #ifdef AE_SIMD_SimdDouble2
		if constexpr( isI64 or isU64 )
			return ToDouble().ToFloat();
		else
	  #endif
			return ToInt<Idx>().ToFloat();
	}
# endif
/*
=================================================
	ToDouble
=================================================
*/
# ifdef AE_SIMD_SimdDouble2
	template <typename IT>
	template <uint Idx>
	auto  SimdTInt128<IT>::ToDouble () C_NE___
	{
		if constexpr( isI64 ){
			StaticAssert( Idx == 0 );
			return SimdDouble2{ vcvtq_f64_s64( _value )};
		}else
		if constexpr( isU64 ){
			StaticAssert( Idx == 0 );
			return SimdDouble2{ vcvtq_f64_u64( _value )};
		}else
			return ToLong<Idx>().ToDouble();
	}
# endif
/*
=================================================
	ToByte
=================================================
*/
	template <typename IT>
	auto  SimdTInt128<IT>::ToByte () C_NE___
	{
		StaticAssert( sizeof(Scalar_t) != sizeof(ubyte) );

		if constexpr( isI16 )
			return SimdByte8{  vmovn_s16( _value )};
		else
		if constexpr( isU16 )
			return SimdUByte8{ vmovn_u16( _value )};
		else
		if constexpr( isI32 or isU32 )
			return ToShort().ToByte();
		else
		if constexpr( isI64 or isU64 )
			return ToInt().ToShort().ToByte();
	}

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

		if constexpr( isI8 )
		{
			StaticAssert( Idx < 2 );
			if constexpr( Idx == 0 )
				return SimdShort8{ vmovl_s8( vget_low_s8( _value ))};
			else
				return SimdShort8{ vmovl_s8( vget_high_s8( _value ))};
		}else
		if constexpr( isU8 )
		{
			StaticAssert( Idx < 2 );
			if constexpr( Idx == 0 )
				return SimdUShort8{ vmovl_u8( vget_low_u8( _value ))};
			else
				return SimdUShort8{ vmovl_u8( vget_high_u8( _value ))};
		}else
		if constexpr( isI32 or isU32 )
		{
			StaticAssert( Idx == 0 );
			if constexpr( isI32 )	return SimdShort4{  vmovn_s32( _value )};
			if constexpr( isU32 )	return SimdUShort4{ vmovn_u32( _value )};
		}else
		if constexpr( isI64 or isU64 )
		{
			StaticAssert( Idx == 0 );
			return ToInt().ToShort();
		}
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

		if constexpr( isI16 )
		{
			StaticAssert( Idx < 2 );
			if constexpr( Idx == 0 )
				return SimdInt4{ vmovl_s16( vget_low_s16( _value ))};
			else
				return SimdInt4{ vmovl_s16( vget_high_s16( _value ))};
		}else
		if constexpr( isU16 )
		{
			StaticAssert( Idx < 2 );
			if constexpr( Idx == 0 )
				return SimdUInt4{ vmovl_u16( vget_low_u16( _value ))};
			else
				return SimdUInt4{ vmovl_u16( vget_high_u16( _value ))};
		}else
		if constexpr( isU8 or isI8 )
		{
			StaticAssert( Idx < 4 );
			return ToShort< Idx/2 >().template ToInt< Idx&1 >();
		}else
		if constexpr( isI64 or isU64 )
		{
			StaticAssert( Idx == 0 );
			if constexpr( isI64 )	return SimdInt2{  vmovn_s64( _value )};
			if constexpr( isU64 )	return SimdUInt2{ vmovn_u64( _value )};
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

		if constexpr( isU32 )
		{
			StaticAssert( Idx < 2 );
			if constexpr( Idx == 0 )
				return SimdULong2{ vmovl_u32( vget_low_u32( _value ))};
			else
				return SimdULong2{ vmovl_u32( vget_high_u32( _value ))};
		}else
		if constexpr( isI32 )
		{
			StaticAssert( Idx < 2 );
			if constexpr( Idx == 0 )
				return SimdLong2{ vmovl_s32( vget_low_s32( _value ))};
			else
				return SimdLong2{ vmovl_s32( vget_high_s32( _value ))};
		}else
		if constexpr( isU16 or isI16 )
		{
			StaticAssert( Idx < 4 );
			return ToInt< Idx/2 >().template ToLong< Idx&1 >();
		}else
		if constexpr( isU8 or isI8 )
		{
			StaticAssert( Idx < 8 );
			return ToShort< Idx/4 >().template ToInt< (Idx&3)/2 >().template ToLong< Idx&1 >();
		}
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

		if constexpr( IsSame< DstScalar, half >)
		{
		  #if defined(AE_SIMD_SimdHalf4) or defined(AE_SIMD_SimdHalf8)
			if constexpr( isI32 or isU32 )	return ToHalf();
			if constexpr( isI64 or isU64 )	return ToHalf();
		  #endif
		  #ifdef AE_SIMD_SimdHalf8
			if constexpr( isI16 or isU16 )	return ToHalf();
			if constexpr( isI8  or isU8 )	return std::array{ ToHalf<0>(), ToHalf<1>() };
		  #endif
		}

		#ifdef AE_SIMD_SimdFloat4
		if constexpr( IsSame< DstScalar, float >)
		{
			if constexpr( isI8  or isU8  )	return std::array{ ToFloat<0>(), ToFloat<1>(), ToFloat<2>(), ToFloat<3>() };
			if constexpr( isI16 or isU16 )	return std::array{ ToFloat<0>(), ToFloat<1>() };
			if constexpr( isI32 or isU32 )	return ToFloat();
			if constexpr( isI64 or isU64 )	return ToFloat();
		}
		#endif

		#ifdef AE_SIMD_SimdDouble2
		if constexpr( IsSame< DstScalar, double >)
		{
			if constexpr( isI64 or isU64 )	return ToDouble();
			if constexpr( isI32 or isU32 )	return std::array{	ToDouble<0>(), ToDouble<1>() };
			if constexpr( isI16 or isU16 )	return std::array{	ToDouble<0>(), ToDouble<1>(), ToDouble<2>(), ToDouble<3>() };
			if constexpr( isI8  or isU8  )	return std::array{	ToDouble<0>(), ToDouble<1>(), ToDouble<2>(), ToDouble<3>(),
																ToDouble<4>(), ToDouble<5>(), ToDouble<6>(), ToDouble<7>() };
		}
		#endif

		if constexpr( IsInteger< DstScalar >)
		{
			using Dst  = SimdTInt128<DstScalar>;
			using Dst2 = SimdTInt64<DstScalar>;

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
				if constexpr( isI64 )	return ToInt().template BitCast<Dst2>();
				if constexpr( isU64 )	return ToInt();
			}
			if constexpr( IsSame< DstScalar, sint >)
			{
				if constexpr( isI8 )	return std::array{	ToInt<0>(), ToInt<1>(), ToInt<2>(), ToInt<3>() };
				if constexpr( isU8 )	return std::array{	ToInt<0>().template BitCast<Dst>(), ToInt<1>().template BitCast<Dst>(),
															ToInt<2>().template BitCast<Dst>(), ToInt<3>().template BitCast<Dst>() };
				if constexpr( isI16 )	return std::array{	ToInt<0>(), ToInt<1>() };
				if constexpr( isU16 )	return std::array{	ToInt<0>().template BitCast<Dst>(), ToInt<1>().template BitCast<Dst>() };
				if constexpr( isU32 )	return BitCast<Dst>();
				if constexpr( isI64 )	return ToInt();
				if constexpr( isU64 )	return ToInt().template BitCast<Dst2>();
			}

			if constexpr( IsSame< DstScalar, ushort >)
			{
				if constexpr( isU8  )			return std::array{	ToShort<0>(), ToShort<1>() };
				if constexpr( isI8  )			return std::array{	ToShort<0>().template BitCast<Dst>(), ToShort<1>().template BitCast<Dst>() };
				if constexpr( isI16 )			return BitCast<Dst>();
				if constexpr( isU32 or isU64 )	return ToShort();
				if constexpr( isI32 or isI64 )	return ToShort().template BitCast<Dst2>();
			}
			if constexpr( IsSame< DstScalar, sshort >)
			{
				if constexpr( isI8  )			return std::array{	ToShort<0>(), ToShort<1>() };
				if constexpr( isU8  )			return std::array{	ToShort<0>().template BitCast<Dst>(), ToShort<1>().template BitCast<Dst>() };
				if constexpr( isU16 )			return BitCast<Dst>();
				if constexpr( isI32 or isI64 )	return ToShort();
				if constexpr( isU32 or isU64 )	return ToShort().template BitCast<Dst2>();
			}

			if constexpr( IsSame< DstScalar, ubyte >)
			{
				if constexpr( isI8 )					return BitCast<Dst>();
				if constexpr( isU16 or isU32 or isU64 )	return ToByte();
				if constexpr( isI16 or isI32 or isI64 )	return ToByte().template BitCast<Dst2>();
			}
			if constexpr( IsSame< DstScalar, sbyte >)
			{
				if constexpr( isU8 )					return BitCast<Dst>();
				if constexpr( isU16 or isU32 or isU64 )	return ToByte().template BitCast<Dst2>();
				if constexpr( isI16 or isI32 or isI64 )	return ToByte();
			}
		}
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
	  #if AE_SIMD_NEON_HALF
		if constexpr( IsSame< DstScalar, half >)
			return true;
		else
	  #endif
	  #ifdef AE_SIMD_SimdFloat4
		if constexpr( IsSame< DstScalar, float >)
			return true;
		else
	  #endif
	  #ifdef AE_SIMD_SimdDouble2
		if constexpr( IsSame< DstScalar, double >)
			return true;
		else
	  #endif
		if constexpr( IsInteger< DstScalar >)
			return true;
		else
			return false;
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

		if constexpr( isI8 )
		{
			if constexpr( IsSame< DstType, Int128b >)		return Int128b{ vreinterpretq_u8_s8( _value )};
			if constexpr( IsSame< DstType, SimdUByte16 >)	return SimdUByte16{ vreinterpretq_u8_s8( _value )};
		}
		if constexpr( isU8 )
		{
			if constexpr( IsSame< DstType, Int128b >)		return Int128b{ _value };
			if constexpr( IsSame< DstType, SimdByte16 >)	return SimdByte16{ vreinterpretq_s8_u8( _value )};
		}
		if constexpr( isI16 )
		{
			if constexpr( IsSame< DstType, Int128b >)		return Int128b{ vreinterpretq_u8_s16( _value )};
			if constexpr( IsSame< DstType, SimdUShort8 >)	return SimdUShort8{ vreinterpretq_u16_s16( _value )};
		  #ifdef AE_SIMD_SimdHalf8
			if constexpr( IsSame< DstType, SimdHalf8 >)		return SimdHalf8{ vreinterpretq_f16_s16( _value )};
		  #endif
		}
		if constexpr( isU16 )
		{
			if constexpr( IsSame< DstType, Int128b >)		return Int128b{ vreinterpretq_u8_u16( _value )};
			if constexpr( IsSame< DstType, SimdShort8 >)	return SimdShort8{ vreinterpretq_s16_u16( _value )};
		  #ifdef AE_SIMD_SimdHalf8
			if constexpr( IsSame< DstType, SimdHalf8 >)		return SimdHalf8{ vreinterpretq_f16_u16( _value )};
		  #endif
		}
		if constexpr( isI32 )
		{
			if constexpr( IsSame< DstType, Int128b >)		return Int128b{ vreinterpretq_u8_s32( _value )};
			if constexpr( IsSame< DstType, SimdFloat4 >)	return SimdFloat4{ vreinterpretq_f32_s32( _value )};
			if constexpr( IsSame< DstType, SimdUInt4 >)		return SimdUInt4{ vreinterpretq_u32_s32( _value )};
		}
		if constexpr( isU32 )
		{
			if constexpr( IsSame< DstType, Int128b >)		return Int128b{ vreinterpretq_u8_u32( _value )};
			if constexpr( IsSame< DstType, SimdFloat4 >)	return SimdFloat4{ vreinterpretq_f32_u32( _value )};
			if constexpr( IsSame< DstType, SimdInt4 >)		return SimdInt4{ vreinterpretq_s32_u32( _value )};
		}
		if constexpr( isI64 )
		{
			if constexpr( IsSame< DstType, Int128b >)		return Int128b{ vreinterpretq_u8_s64( _value )};
			if constexpr( IsSame< DstType, SimdULong2 >)	return SimdULong2{ vreinterpretq_u64_s64( _value )};
		  #ifdef AE_SIMD_SimdDouble2
			if constexpr( IsSame< DstType, SimdDouble2 >)	return SimdDouble2{ vreinterpretq_f64_s64( _value )};
		  #endif
		}
		if constexpr( isU64 )
		{
			if constexpr( IsSame< DstType, Int128b >)		return Int128b{ vreinterpretq_u8_u64( _value )};
			if constexpr( IsSame< DstType, SimdLong2 >)		return SimdLong2{ vreinterpretq_s64_u64( _value )};
		  #ifdef AE_SIMD_SimdDouble2
			if constexpr( IsSame< DstType, SimdDouble2 >)	return SimdDouble2{ vreinterpretq_f64_u64( _value )};
		  #endif
		}
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
	  #ifdef AE_SIMD_SimdHalf8
		if constexpr( IsSame< DstType, SimdHalf8 > and sizeof(IT) == sizeof(half) )
			return true;
		else
	  #endif
		if constexpr( IsSame< DstType, SimdFloat4 > and sizeof(IT) == sizeof(float) )
			return true;
		else
	  #ifdef AE_SIMD_SimdDouble2
		if constexpr( IsSame< DstType, SimdDouble2 > and sizeof(IT) == sizeof(double) )
			return true;
		else
	  #endif
		if constexpr( IsSpecializationOf< DstType, SimdTInt128 >)
			return true;
		else
		if constexpr( IsSame< DstType, Int128b >)
			return true;
		else
			return false;
	}

#endif // AE_SIMD_SimdTInt128
//-----------------------------------------------------------------------------



#ifdef AE_SIMD_SimdHalf8
/*
=================================================
	Bool8: BitCast
=================================================
*/
	template <typename DstType>
	DstType  SimdHalf8::Bool8::BitCast () C_NE___
	{
		if constexpr( IsSame< DstType, SimdUShort8 >)
			return SimdUShort8{ _value };
		else
			return SimdUShort8{ _value }.BitCast< DstType >();
	}

/*
=================================================
	constructor
=================================================
*/
	inline SimdHalf8::SimdHalf8 (Scalar_t v0, Scalar_t v1, Scalar_t v2, Scalar_t v3,
								 Scalar_t v4, Scalar_t v5, Scalar_t v6, Scalar_t v7) __NE___
	{
		Scalar_t a[] = {v0, v1, v2, v3, v4, v5, v6, v7};
		_value = vld1q_f16( Base::Cast<float16_t>(a) );
	}

	inline SimdHalf8::SimdHalf8 (float16_t v0, float16_t v1, float16_t v2, float16_t v3,
								 float16_t v4, float16_t v5, float16_t v6, float16_t v7) __NE___
	{
		float16_t a[] = {v0, v1, v2, v3, v4, v5, v6, v7};
		_value = vld1q_f16( a );
	}

	inline SimdHalf8::SimdHalf8 (const SimdFloat4 &v0123) __NE___ :
		SimdHalf8{ SimdHalf4{ v0123 }}
	{}

	inline SimdHalf8::SimdHalf8 (const SimdFloat4 &v0123, const SimdFloat4 &v4567) __NE___
	{
		float16x4_t	low  = vcvt_f16_f32( v0123.Ref() );
		float16x4_t	high = vcvt_f16_f32( v4567.Ref() );
		_value = vcombine_f16( low, high );
	}

	inline SimdHalf8::SimdHalf8 (float v0, float v1, float v2, float v3, float v4, float v5, float v6, float v7) __NE___ :
		SimdHalf8{ SimdFloat4{ v0, v1, v2, v3 }, SimdFloat4{ v4, v5, v6, v7 }}
	{}

/*
=================================================
	ToFloat
=================================================
*/
	template <uint I>
	auto  SimdHalf8::ToFloat ()  C_NE___
	{
		StaticAssert( I < 2 );
		if constexpr( I == 0 )
		{
			return SimdFloat4{ vcvt_f32_f16( vget_low_f16( _value ))};
		}
		else
		{
		#if AE_SIMD_NEON64
			return SimdFloat4{ vcvt_high_f32_f16( _value )};
		#else
			return SimdFloat4{ vcvt_f32_f16( vget_high_f16( _value ))};
		#endif
		}
	}

/*
=================================================
	Convert
----
	same as C++ style conversion 'Dst(src)'
=================================================
*/
	template <typename DstScalar>
	auto  SimdHalf8::Convert ()  C_NE___
	{
		StaticAssert( IsAnyScalar< DstScalar >);
		StaticAssert( not IsSame< DstScalar, Scalar_t >);
		StaticAssert( Has_Convert< DstScalar >() );

		if constexpr( IsSame< DstScalar, float >)
			return std::array{ ToFloat<0>(), ToFloat<1>() };

		if constexpr( IsSame< DstScalar, sshort >)
			return SimdShort8{ vcvtq_s16_f16( _value )};

		if constexpr( IsSame< DstScalar, ushort >)
			return SimdUShort8{ vcvtq_u16_f16( _value )};
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
		if constexpr( IsSame< DstScalar, sshort > or IsSame< DstScalar, ushort >)
			return true;
		else
			return false;
	}

/*
=================================================
	BitCast
=================================================
*/
	template <typename DstType>
	DstType  SimdHalf8::BitCast ()  C_NE___
	{
		StaticAssert( sizeof(Self) == sizeof(DstType) );
		StaticAssert( Has_BitCast<DstType>() );

		if constexpr( IsSame< DstType, SimdInt_t > or IsSame< DstType, SimdUInt_t >)
			return DstType{ vreinterpretq_u16_f16( _value )};
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
	Part
=================================================
*/
	template <uint Idx>
	auto  SimdHalf8::Part ()  C_NE___
	{
		StaticAssert( Idx < parts );
		if constexpr( Idx == 0 )	return SimdHalf4{ vget_low_f16(  _value )};
		else						return SimdHalf4{ vget_high_f16( _value )};
	}

#endif // AE_SIMD_SimdHalf8
//-----------------------------------------------------------------------------



#ifdef AE_SIMD_SimdFloat4
/*
=================================================
	Bool4: BitCast
=================================================
*/
	template <typename DstType>
	DstType  SimdFloat4::Bool4::BitCast () C_NE___
	{
		if constexpr( IsSame< DstType, SimdUInt4 >)
			return SimdUInt4{ _value };
		else
			return SimdUInt4{ _value }.BitCast< DstType >();
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
		return Self{ __builtin_shufflevector( _value, _value, X, Y, Z, W )};
	}

/*
=================================================
	Shuffle
=================================================
*/
	template <uint X, uint Y, uint Z, uint W>
	SimdFloat4  SimdFloat4::Shuffle (const Self &v4567)  C_NE___
	{
		StaticAssert( Has_Shuffle() );
		StaticAssert( X < count*2 );
		StaticAssert( Y < count*2 );
		StaticAssert( Z < count*2 );
		StaticAssert( W < count*2 );

	  #if AE_SIMD_NEON64
		alignas(16) static const ubyte  indices [16] =
		{
			ubyte(X*4+0), ubyte(X*4+1), ubyte(X*4+2), ubyte(X*4+3),
			ubyte(Y*4+0), ubyte(Y*4+1), ubyte(Y*4+2), ubyte(Y*4+3),
			ubyte(Z*4+0), ubyte(Z*4+1), ubyte(Z*4+2), ubyte(Z*4+3),
			ubyte(W*4+0), ubyte(W*4+1), ubyte(W*4+2), ubyte(W*4+3)
		};

		uint8x16x2_t	table;
		table.val[0] = vreinterpretq_u8_f32( _value );
		table.val[1] = vreinterpretq_u8_f32( v4567._value );

		return Self{ vreinterpretq_f32_u8( vqtbl2q_u8( table, vld1q_u8( indices )))};

	  #else
		alignas(8) static const ubyte  indices01 [8] =
		{
			ubyte(X*4+0), ubyte(X*4+1), ubyte(X*4+2), ubyte(X*4+3),
			ubyte(Y*4+0), ubyte(Y*4+1), ubyte(Y*4+2), ubyte(Y*4+3)
		};
		alignas(8) static const ubyte  indices23 [8] =
		{
			ubyte(Z*4+0), ubyte(Z*4+1), ubyte(Z*4+2), ubyte(Z*4+3),
			ubyte(W*4+0), ubyte(W*4+1), ubyte(W*4+2), ubyte(W*4+3)
		};

		uint8x8x4_t		table;
		table.val[0] = vreinterpret_u8_f32( vget_low_f32( _value ));
		table.val[1] = vreinterpret_u8_f32( vget_high_f32( _value ));
		table.val[2] = vreinterpret_u8_f32( vget_low_f32( v4567._value ));
		table.val[3] = vreinterpret_u8_f32( vget_high_f32( v4567._value ));

		float32x2_t		lo = vreinterpret_f32_u8( vtbl4_u8( table, vld1_u8( indices01 )));
		float32x2_t		hi = vreinterpret_f32_u8( vtbl4_u8( table, vld1_u8( indices23 )));

		return Self{ vcombine_f32( lo, hi )};
	  #endif
	}

/*
=================================================
	ToDouble / ToInt / ToHalf
=================================================
*/
	inline SimdInt4  SimdFloat4::ToInt () C_NE___
	{
		return SimdInt4{ vcvtq_s32_f32( _value )};
	}

# ifdef AE_SIMD_SimdDouble2
	template <uint Idx>
	inline SimdDouble2  SimdFloat4::ToDouble () C_NE___
	{
		StaticAssert( Idx < 2 );
		if constexpr( Idx == 0 )
			return SimdDouble2{ vcvt_f64_f32( vget_low_f32( _value ))};
		else
			return SimdDouble2{ vcvt_f64_f32( vget_high_f32( _value ))};
	}
# endif
# ifdef AE_SIMD_SimdHalf4
	inline SimdHalf4  SimdFloat4::ToHalf () C_NE___
	{
		return SimdHalf4{ vcvt_f16_f32( _value )};
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

	  #ifdef AE_SIMD_SimdHalf4
		if constexpr( IsSame< DstScalar, half >)
			return ToHalf();
	  #endif
	  #ifdef AE_SIMD_SimdDouble2
		if constexpr( IsSame< DstScalar, double >)
			return ToDouble();
	  #endif

		if constexpr( IsSame< DstScalar, int >)
			return SimdInt4{ vcvtq_s32_f32( _value )};

		if constexpr( IsSame< DstScalar, uint >)
			return SimdUInt4{ vcvtq_u32_f32( _value )};
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
	  #ifdef AE_SIMD_SimdHalf4
		if constexpr( IsSame< DstScalar, half >)
			return true;
		else
	  #endif
		if constexpr( IsSame< DstScalar, int > or IsSame< DstScalar, uint >)
			return true;
		else
			return false;
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

		if constexpr( IsSame< DstType, SimdInt4 >)
			return SimdInt4{ vreinterpretq_s32_f32( _value )};

		if constexpr( IsSame< DstType, SimdUInt4 >)
			return SimdUInt4{ vreinterpretq_u32_f32( _value )};

		if constexpr( IsSame< DstType, SimdLong2 >)
			return SimdLong2{ vreinterpretq_s64_f32( _value )};

		if constexpr( IsSame< DstType, SimdULong2 >)
			return SimdULong2{ vreinterpretq_u64_f32( _value )};

		if constexpr( IsSame< DstType, SimdShort8 >)
			return SimdShort8{ vreinterpretq_s16_f32( _value )};

		if constexpr( IsSame< DstType, SimdUShort8 >)
			return SimdUShort8{ vreinterpretq_u16_f32( _value )};

		if constexpr( IsSame< DstType, SimdByte16 >)
			return SimdByte16{ vreinterpretq_s8_f32( _value )};

		if constexpr( IsSame< DstType, SimdUByte16 >)
			return SimdUByte16{ vreinterpretq_u8_f32( _value )};

		if constexpr( IsSame< DstType, Int128b >)
			return Int128b{ vreinterpretq_u8_f32( _value )};
	}

/*
=================================================
	Has_BitCast
=================================================
*/
	template <typename DstType>
	__Ce__ bool  SimdFloat4::Has_BitCast ()
	{
		if constexpr( IsSpecializationOf< DstType, SimdTInt128 >)
			return true;
		else
		if constexpr( IsSame< DstType, Int128b >)
			return true;
		else
			return false;
	}

/*
=================================================
	InclusiveAdd
=================================================
*/
	inline SimdFloat4  SimdFloat4::InclusiveAdd ()  C_NE___
	{
		StaticAssert( Has_InclusiveAdd() );

		float32x4_t v = _value;											// [x0, x1, x2, x3]

		float32x4_t t = vsetq_lane_f32( 0.0f, vextq_f32( v, v, 3 ), 0 );
		v = vaddq_f32( v, t );											 // [x0, x0+x1, x1+x2, x2+x3]

		t = vcombine_f32( vdup_n_f32(0.0f), vget_low_f32(v) );
		v = vaddq_f32( v, t );											// [x0, x0+x1, x0+x1+x2, x0+x1+x2+x3]

		return Self{v};
	}

/*
=================================================
	ExclusiveAdd
=================================================
*/
	inline SimdFloat4  SimdFloat4::ExclusiveAdd ()  C_NE___
	{
		StaticAssert( Has_InclusiveAdd() );

		float32x4_t			v		= _value;				// [x0, x1, x2, x3]
		const float32x4_t	zero	= vdupq_n_f32( 0.0f );

		float32x4_t t = vextq_f32( zero, v, 3 );			// [0, x0, x1, x2]
		v = vaddq_f32( v, t );								// [x0, x0+x1, x1+x2, x2+x3]

		t = vextq_f32( zero, v, 2 );						// [0, 0, x0, x0+x1]
		v = vaddq_f32( v, t );								// [x0, x0+x1, x0+x1+x2, x0+x1+x2+x3]

		return Self{ vextq_f32( zero, v, 3 )};				// [0, x0, x0+x1, x0+x1+x2]
	}

#endif // AE_SIMD_SimdFloat4
//-----------------------------------------------------------------------------



#ifdef AE_SIMD_SimdDouble2
/*
=================================================
	Bool2: BitCast
=================================================
*/
	template <typename DstType>
	DstType  SimdDouble2::Bool2::BitCast () C_NE___
	{
		if constexpr( IsSame< DstType, SimdULong2 >)
			return SimdULong2{ _value };
		else
			return SimdULong2{ _value }.BitCast< DstType >();
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
		return Self{ __builtin_shufflevector( _value, _value, X, Y )};
	}

/*
=================================================
	Shuffle
=================================================
*/
	template <uint X, uint Y>
	SimdDouble2  SimdDouble2::Shuffle (const Self &v23)  C_NE___
	{
		StaticAssert( Has_Shuffle() );
		StaticAssert( X < count*2 );
		StaticAssert( Y < count*2 );

		const float64x2_t	a = (X < count ? _value : v23._value);	// source of result lane 0
		const float64x2_t	b = (Y < count ? _value : v23._value);	// source of result lane 1

		if constexpr( (X % count) == 0 and (Y % count) == 0 )
			return Self{ vtrn1q_f64( a, b ) };										// { a0, b0 }
		else
		if constexpr( (X % count) == 1 and (Y % count) == 1 )
			return Self{ vtrn2q_f64( a, b ) };										// { a1, b1 }
		else
		if constexpr( (X % count) == 0 )
			return Self{ vcombine_f64( vget_low_f64( a ), vget_high_f64( b ) ) };	// { a0, b1 }
		else
			return Self{ vcombine_f64( vget_high_f64( a ), vget_low_f64( b ) ) };	// { a1, b0 }
	}

/*
=================================================
	ToFloat / ToInt / ToLong
=================================================
*/
	inline SimdFloat4  SimdDouble2::ToFloat () C_NE___
	{
		return SimdFloat4{ vcvt_f32_f64( _value )};
	}

	inline SimdInt2  SimdDouble2::ToInt () C_NE___
	{
		return ToLong().ToInt();
	}

	inline SimdLong2  SimdDouble2::ToLong () C_NE___
	{
		return SimdLong2{ vcvtq_s64_f64( _value )};
	}

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

		if constexpr( IsSame< DstScalar, slong >)
			return ToLong();

		if constexpr( IsSame< DstScalar, ulong >)
			return SimdULong2{ vcvtq_u64_f64( _value )};

		if constexpr( IsSame< DstScalar, sint >)
			return ToLong().ToInt();

		if constexpr( IsSame< DstScalar, uint >)
			return SimdULong2{ vcvtq_u64_f64( _value )}.ToInt();

		if constexpr( IsSame< DstScalar, float >)
			return ToFloat();

	  #ifdef AE_SIMD_SimdHalf4
		if constexpr( IsSame< DstScalar, half >)
			return ToFloat().ToHalf();
	  #endif
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
		if constexpr( IsInteger<DstScalar> and (sizeof(DstScalar) == 4 or sizeof(DstScalar) == 8) )
			return true;
		else
		if constexpr( IsSame< DstScalar, float >)
			return true;
		else
	  #ifdef AE_SIMD_SimdHalf4
		if constexpr( IsSame< DstScalar, half >)
			return true;
		else
	  #endif
			return false;
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

		if constexpr( IsSame< DstType, SimdLong2 >)
			return SimdLong2{ vreinterpretq_s64_f64( _value )};

		if constexpr( IsSame< DstType, SimdULong2 >)
			return SimdULong2{ vreinterpretq_u64_f64( _value )};

		if constexpr( IsSame< DstType, SimdInt4 >)
			return SimdInt4{ vreinterpretq_s32_f64( _value )};

		if constexpr( IsSame< DstType, SimdUInt4 >)
			return SimdUInt4{ vreinterpretq_u32_f64( _value )};

		if constexpr( IsSame< DstType, SimdShort8 >)
			return SimdShort8{ vreinterpretq_s16_f64( _value )};

		if constexpr( IsSame< DstType, SimdUShort8 >)
			return SimdUShort8{ vreinterpretq_u16_f64( _value )};

		if constexpr( IsSame< DstType, SimdByte16 >)
			return SimdByte16{ vreinterpretq_s8_f64( _value )};

		if constexpr( IsSame< DstType, SimdUByte16 >)
			return SimdUByte16{ vreinterpretq_u8_f64( _value )};

		if constexpr( IsSame< DstType, Int128b >)
			return Int128b{ vreinterpretq_u8_f64( _value )};
	}

/*
=================================================
	Has_BitCast
=================================================
*/
	template <typename DstType>
	__Ce__ bool  SimdDouble2::Has_BitCast ()
	{
		if constexpr( IsSpecializationOf< DstType, SimdTInt128 >)
			return true;
		else
		if constexpr( IsSame< DstType, Int128b >)
			return true;
		else
			return false;
	}

/*
=================================================
	InclusiveAdd
=================================================
*/
	inline SimdDouble2  SimdDouble2::InclusiveAdd ()  C_NE___
	{
		StaticAssert( Has_InclusiveAdd() );

		float64x2_t	v = _value;										// [x0, x1]
		float64x2_t	t = vextq_f64( vdupq_n_f64( 0.0 ), v, 1 );		// [0, x0]
		return Self{ vaddq_f64( v, t )};							// [x0, x0+x1]
	}

/*
=================================================
	ExclusiveAdd
=================================================
*/
	inline SimdDouble2  SimdDouble2::ExclusiveAdd ()  C_NE___
	{
		StaticAssert( Has_InclusiveAdd() );

		float64x2_t	v = _value;										// [x0, x1]
		return Self{ vextq_f64( vdupq_n_f64( 0.0 ), v, 1 )};		// [0, x0]
	}

#endif // AE_SIMD_SimdDouble2
//-----------------------------------------------------------------------------


#ifdef AE_SIMD_Int128b
#endif // AE_SIMD_Int128b
//-----------------------------------------------------------------------------

} // AE::Base
