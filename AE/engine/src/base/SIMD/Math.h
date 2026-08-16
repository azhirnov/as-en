// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#pragma once

#include "base/Math/Vec.h"
#include "base/Math/BitMath.h"
#include "base/Math/FloatConversion.h"
#include "base/Pointers/AlignedPtr.h"

// fallback to scalar
#define AE_SIMD_ENABLE_FALLBACK		1

namespace AE::Base
{
	struct SimdHalf4;
	template <typename IntType> struct SimdTInt64;
	using SimdByte8		= SimdTInt64< sbyte >;
	using SimdUByte8	= SimdTInt64< ubyte >;
	using SimdShort4	= SimdTInt64< sshort >;
	using SimdUShort4	= SimdTInt64< ushort >;
	using SimdInt2		= SimdTInt64< sint >;
	using SimdUInt2		= SimdTInt64< uint >;

	struct Int128b;
	struct SimdHalf8;
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
	struct SimdHalf16;
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
	struct SimdHalf32;
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

} // AE::Base

#include "base/SIMD/MSBMask.h"
#include "base/SIMD/X64_SIMD.h"
#include "base/SIMD/ARM_SIMD.h"
#include "base/SIMD/FloatConversion.h"


namespace AE::Base
{
/*
=================================================
	All/Any
=================================================
*/
#ifdef AE_SIMD_SimdHalf4
	Nd__In bool  All (const SimdHalf4::Bool4 &v)					__NE___	{ return v.All(); }
	Nd__In bool  Any (const SimdHalf4::Bool4 &v)					__NE___	{ return v.Any(); }
#endif
#ifdef AE_SIMD_SimdTInt64
	template <typename T> ND_ bool  All (const SimdTInt64<T> &v)	__NE___	{ return v.All(); }
	template <typename T> ND_ bool  Any (const SimdTInt64<T> &v)	__NE___	{ return v.Any(); }
#endif
#ifdef AE_SIMD_SimdTInt128
	template <typename T> ND_ bool  All (const SimdTInt128<T> &v)	__NE___	{ return v.All(); }
	template <typename T> ND_ bool  Any (const SimdTInt128<T> &v)	__NE___	{ return v.Any(); }
#endif
#ifdef AE_SIMD_SimdHalf8
	Nd__In bool  All (const SimdHalf8::Bool8 &v)					__NE___	{ return v.All(); }
	Nd__In bool  Any (const SimdHalf8::Bool8 &v)					__NE___	{ return v.Any(); }
#endif
#ifdef AE_SIMD_SimdFloat4
	Nd__In bool  All (const SimdFloat4::Bool4 &v)					__NE___	{ return v.All(); }
	Nd__In bool  Any (const SimdFloat4::Bool4 &v)					__NE___	{ return v.Any(); }
#endif
#ifdef AE_SIMD_SimdDouble2
	Nd__In bool  All (const SimdDouble2::Bool2 &v)					__NE___	{ return v.All(); }
	Nd__In bool  Any (const SimdDouble2::Bool2 &v)					__NE___	{ return v.Any(); }
#endif
#ifdef AE_SIMD_SimdTInt256
	template <typename T> ND_ bool  All (const SimdTInt256<T> &v)	__NE___	{ return v.All(); }
	template <typename T> ND_ bool  Any (const SimdTInt256<T> &v)	__NE___	{ return v.Any(); }
#endif
#ifdef AE_SIMD_SimdFloat8
	Nd__In bool  All (const SimdFloat8::Bool8 &v)					__NE___	{ return v.All(); }
	Nd__In bool  Any (const SimdFloat8::Bool8 &v)					__NE___	{ return v.Any(); }
#endif
#ifdef AE_SIMD_SimdDouble4
	Nd__In bool  All (const SimdDouble4::Bool4 &v)					__NE___	{ return v.All(); }
	Nd__In bool  Any (const SimdDouble4::Bool4 &v)					__NE___	{ return v.Any(); }
#endif
#ifdef AE_SIMD_SimdHalf16
	Nd__In bool  All (const SimdHalf16::Bool16 &v)					__NE___	{ return v.All(); }
	Nd__In bool  Any (const SimdHalf16::Bool16 &v)					__NE___	{ return v.Any(); }
#endif
#ifdef AE_SIMD_SimdTInt512
	template <typename T> ND_ bool  All (const SimdTInt512<T> &v)	__NE___	{ return v.All(); }
	template <typename T> ND_ bool  Any (const SimdTInt512<T> &v)	__NE___	{ return v.Any(); }
#endif
#if 0 //def AE_SIMD_SimdFloat16
	Nd__In bool  All (const SimdFloat16::Bool16 &v)					__NE___	{ return v.All(); }
	Nd__In bool  Any (const SimdFloat16::Bool16 &v)					__NE___	{ return v.Any(); }
#endif
#if 0 //def AE_SIMD_SimdDouble8
	Nd__In bool  All (const SimdDouble8::Bool8 &v)					__NE___	{ return v.All(); }
	Nd__In bool  Any (const SimdDouble8::Bool8 &v)					__NE___	{ return v.Any(); }
#endif

/*
=================================================
	VecSize
=================================================
*/
namespace _hidden_
{
#ifdef AE_SIMD_SimdHalf4
	template <>
	struct _VecInfo< SimdHalf4 > {
		static constexpr bool	is_vec	= false;	// ?
		static constexpr bool	is_simd	= true;
		static constexpr bool	is_glm	= false;
		static constexpr int	size	= 4;
	};
#endif
#ifdef AE_SIMD_SimdTInt64
	template <typename T>
	struct _VecInfo< SimdTInt64<T> > {
		static constexpr bool	is_vec	= false;	// ?
		static constexpr bool	is_simd	= true;
		static constexpr bool	is_glm	= false;
		static constexpr int	size	= SimdTInt64<T>::count;
	};
#endif
#ifdef AE_SIMD_SimdTInt128
	template <typename T>
	struct _VecInfo< SimdTInt128<T> > {
		static constexpr bool	is_vec	= false;	// ?
		static constexpr bool	is_simd	= true;
		static constexpr bool	is_glm	= false;
		static constexpr int	size	= SimdTInt128<T>::count;
	};
#endif
#ifdef AE_SIMD_SimdHalf8
	template <>
	struct _VecInfo< SimdHalf8 > {
		static constexpr bool	is_vec	= false;	// ?
		static constexpr bool	is_simd	= true;
		static constexpr bool	is_glm	= false;
		static constexpr int	size	= 8;
	};
#endif
#ifdef AE_SIMD_SimdFloat4
	template <>
	struct _VecInfo< SimdFloat4 > {
		static constexpr bool	is_vec	= false;	// ?
		static constexpr bool	is_simd	= true;
		static constexpr bool	is_glm	= false;
		static constexpr int	size	= 4;
	};
#endif
#ifdef AE_SIMD_SimdDouble2
	template <>
	struct _VecInfo< SimdDouble2 > {
		static constexpr bool	is_vec	= false;	// ?
		static constexpr bool	is_simd	= true;
		static constexpr bool	is_glm	= false;
		static constexpr int	size	= 2;
	};
#endif
#ifdef AE_SIMD_SimdTInt256
	template <typename T>
	struct _VecInfo< SimdTInt256<T> > {
		static constexpr bool	is_vec	= false;	// ?
		static constexpr bool	is_simd	= true;
		static constexpr bool	is_glm	= false;
		static constexpr int	size	= SimdTInt256<T>::count;
	};
#endif
#ifdef AE_SIMD_SimdFloat8
	template <>
	struct _VecInfo< SimdFloat8 > {
		static constexpr bool	is_vec	= false;	// ?
		static constexpr bool	is_simd	= true;
		static constexpr bool	is_glm	= false;
		static constexpr int	size	= 8;
	};
#endif
#ifdef AE_SIMD_SimdDouble4
	template <>
	struct _VecInfo< SimdDouble4 > {
		static constexpr bool	is_vec	= false;	// ?
		static constexpr bool	is_simd	= true;
		static constexpr bool	is_glm	= false;
		static constexpr int	size	= 4;
	};
#endif
#ifdef AE_SIMD_SimdTInt512
	template <typename T>
	struct _VecInfo< SimdTInt512<T> > {
		static constexpr bool	is_vec	= false;	// ?
		static constexpr bool	is_simd	= true;
		static constexpr bool	is_glm	= false;
		static constexpr int	size	= SimdTInt512<T>::count;
	};
#endif
#ifdef AE_SIMD_SimdFloat16
	template <>
	struct _VecInfo< SimdFloat16 > {
		static constexpr bool	is_vec	= false;	// ?
		static constexpr bool	is_simd	= true;
		static constexpr bool	is_glm	= false;
		static constexpr int	size	= 16;
	};
#endif
#ifdef AE_SIMD_SimdDouble8
	template <>
	struct _VecInfo< SimdDouble8 > {
		static constexpr bool	is_vec	= false;	// ?
		static constexpr bool	is_simd	= true;
		static constexpr bool	is_glm	= false;
		static constexpr int	size	= 8;
	};
#endif

	template <typename T>
	static const bool	_HasSimdType =	Base::_hidden_::_VecInfo<T>::is_simd and
										not Base::_hidden_::_VecInfo<T>::is_glm;

} // _hidden_

/*
=================================================
	HasSimdType
=================================================
*/
	template <typename T>
	static const bool	HasSimdType = Base::_hidden_::_HasSimdType< RemoveAllQualifiers<T> >;

/*
=================================================
	Sqrt
----
	used high precision version if supported
=================================================
*/
	template <typename SimdType> requires( HasSimdType<SimdType> )
	ND_ SimdType  Sqrt (const SimdType &v) __NE___
	{
		if constexpr( SimdType::Has_PreciseSqrt() )
			return v.PreciseSqrt();

	  #if AE_SIMD_ENABLE_FALLBACK
		else
		{
			auto	arr = v.ToArray();
			for (auto& a : arr) {
				a = Base::Sqrt( a );
			}
			return SimdType{ arr.data() };
		}
	  #endif
	}

/*
=================================================
	FastSqrt
----
	used low precision version if supported
=================================================
*/
	template <typename SimdType> requires( HasSimdType<SimdType> )
	ND_ SimdType  FastSqrt (const SimdType &v) __NE___
	{
		if constexpr( SimdType::Has_ApproxInvSqrt() )
			return v.FastSqrt();
		else
		if constexpr( SimdType::Has_PreciseSqrt() )
			return v.PreciseSqrt();

	  #if AE_SIMD_ENABLE_FALLBACK
		else
		{
			auto	arr = v.ToArray();
			for (auto& a : arr) {
				a = Base::FastSqrt( a );
			}
			return SimdType{ arr.data() };
		}
	  #endif
	}

/*
=================================================
	InvSqrt
----
	used low precision version if supported
=================================================
*/
	template <typename SimdType> requires( HasSimdType<SimdType> )
	ND_ SimdType  InvSqrt (const SimdType &v) __NE___
	{
		if constexpr( SimdType::Has_ApproxInvSqrt() )
			return v.FastInvSqrt();
		else
		if constexpr( SimdType::Has_PreciseSqrt() )
			return v.PreciseInvSqrt();

	  #if AE_SIMD_ENABLE_FALLBACK
		else
		{
			auto	arr = v.ToArray();
			for (auto& a : arr) {
				a = Base::InvSqrt( a );
			}
			return SimdType{ arr.data() };
		}
	  #endif
	}

/*
=================================================
	Abs
=================================================
*/
	template <typename SimdType>
		requires( HasSimdType<SimdType> and IsSigned<typename SimdType::Scalar_t> )
	ND_ SimdType  Abs (const SimdType &v) __NE___
	{
		return v.Abs();
	}

/*
=================================================
	Min, Max
=================================================
*/
	template <typename SimdType>
		requires( HasSimdType<SimdType> and SimdType::Has_MinMax() )
	ND_ SimdType  Min (const SimdType &a, const SimdType &b) __NE___
	{
		return a.Min( b );
	}

	template <typename SimdType>
		requires( HasSimdType<SimdType> and SimdType::Has_MinMax() )
	ND_ SimdType  Max (const SimdType &a, const SimdType &b) __NE___
	{
		return a.Max( b );
	}

/*
=================================================
	Clamp
=================================================
*/
	template <typename SimdType>
		requires( HasSimdType<SimdType> and SimdType::Has_MinMax() )
	ND_ SimdType  Clamp (const SimdType &value, const SimdType &minValue, const SimdType &maxValue) __NE___
	{
		return Max( Min( value, minValue ), maxValue );
	}

/*
=================================================
	Floor, Ceil, Trunc, Round, RoundEven
=================================================
*/
	template <typename SimdType>
		requires( HasSimdType<SimdType> and SimdType::Has_Rounding() )
	ND_ SimdType  Floor (const SimdType &v) __NE___
	{
		return v.Floor();
	}

	template <typename SimdType>
		requires( HasSimdType<SimdType> and SimdType::Has_Rounding() )
	ND_ SimdType  Ceil (const SimdType &v) __NE___
	{
		return v.Ceil();
	}

	template <typename SimdType>
		requires( HasSimdType<SimdType> and SimdType::Has_Rounding() )
	ND_ SimdType  Trunc (const SimdType &v) __NE___
	{
		return v.Trunc();
	}

	template <typename SimdType>
		requires( HasSimdType<SimdType> and SimdType::Has_Rounding() )
	ND_ SimdType  Round (const SimdType &v) __NE___
	{
		return v.Round();
	}

	template <typename SimdType>
		requires( HasSimdType<SimdType> and SimdType::Has_Rounding() )
	ND_ SimdType  RoundEven (const SimdType &v) __NE___
	{
		return v.RoundEven();
	}

/*
=================================================
	MulAdd / MulSub
=================================================
*/
	template <typename SimdType>
		requires( HasSimdType<SimdType> )
	ND_ SimdType  MulAdd (const SimdType &a, const SimdType &b, const SimdType &c) __NE___
	{
		if constexpr( SimdType::Has_FusedMulAdd() )
			return FusedMulAdd( a, b, c );
		else
			return (a * b) + c;
	}

	template <typename SimdType>
		requires( HasSimdType<SimdType> )
	ND_ SimdType  MulSub (const SimdType &a, const SimdType &b, const SimdType &c) __NE___
	{
		if constexpr( SimdType::Has_FusedMulAdd() )
			return FusedMulSub( a, b, c );
		else
			return (a * b) - c;
	}

/*
=================================================
	NegMulAdd / NegMulSub
=================================================
*/
	template <typename SimdType>
		requires( HasSimdType<SimdType> )
	ND_ SimdType  NegMulAdd (const SimdType &a, const SimdType &b, const SimdType &c) __NE___
	{
		if constexpr( SimdType::Has_FusedMulAdd() )
			return FusedNegMulAdd( a, b, c );
		else
			return c - (a * b);
	}
	template <typename SimdType>
		requires( HasSimdType<SimdType> )
	ND_ SimdType  NegMulSub (const SimdType &a, const SimdType &b, const SimdType &c) __NE___
	{
		if constexpr( SimdType::Has_FusedMulAdd() )
			return FusedNegMulSub( a, b, c );
		else
			return -(a * b) - c;
	}
//-----------------------------------------------------------------------------

# ifdef AE_COMPILER_GCC
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wignored-attributes"
# endif

# ifdef AE_SIMD_SimdFloat4
	template <>				struct TMemCopyAvailable< SimdFloat4 >		: TMemCopyAvailable< SimdFloat4::Native_t >{};
	template <>				struct TZeroMemAvailable< SimdFloat4 >		: TZeroMemAvailable< SimdFloat4::Native_t >{};
	template <>				struct TUnwrap< SimdFloat4 >				: TUnwrap< SimdFloat4::Scalar_t >{};
# endif
# ifdef AE_SIMD_SimdDouble2
	template <>				struct TMemCopyAvailable< SimdDouble2 >		: TMemCopyAvailable< SimdDouble2::Native_t >{};
	template <>				struct TZeroMemAvailable< SimdDouble2 >		: TZeroMemAvailable< SimdDouble2::Native_t >{};
	template <>				struct TUnwrap< SimdDouble2 >				: TUnwrap< SimdDouble2::Scalar_t >{};
# endif
# ifdef AE_SIMD_SimdTInt128
	template <typename T>	struct TMemCopyAvailable< SimdTInt128<T> >	: TMemCopyAvailable< typename SimdTInt128<T>::Native_t >{};
	template <typename T>	struct TZeroMemAvailable< SimdTInt128<T> >	: TZeroMemAvailable< typename SimdTInt128<T>::Native_t >{};
	template <typename T>	struct TUnwrap< SimdTInt128<T> >			: TUnwrap< typename SimdTInt128<T>::Scalar_t >{};
# endif
# ifdef AE_SIMD_SimdFloat8
	template <>				struct TMemCopyAvailable< SimdFloat8 >		: TMemCopyAvailable< SimdFloat8::Native_t >{};
	template <>				struct TZeroMemAvailable< SimdFloat8 >		: TZeroMemAvailable< SimdFloat8::Native_t >{};
	template <>				struct TUnwrap< SimdFloat8 >				: TUnwrap< SimdFloat8::Scalar_t >{};
# endif
# ifdef AE_SIMD_SimdDouble4
	template <>				struct TMemCopyAvailable< SimdDouble4 >		: TMemCopyAvailable< SimdDouble4::Native_t >{};
	template <>				struct TZeroMemAvailable< SimdDouble4 >		: TZeroMemAvailable< SimdDouble4::Native_t >{};
	template <>				struct TUnwrap< SimdDouble4 >				: TUnwrap< SimdDouble4::Scalar_t >{};
# endif
# ifdef AE_SIMD_SimdTInt256
	template <typename T>	struct TMemCopyAvailable< SimdTInt256<T> >	: TMemCopyAvailable< typename SimdTInt256<T>::Native_t >{};
	template <typename T>	struct TZeroMemAvailable< SimdTInt256<T> >	: TZeroMemAvailable< typename SimdTInt256<T>::Native_t >{};
	template <typename T>	struct TUnwrap< SimdTInt256<T> >			: TUnwrap< typename SimdTInt256<T>::Scalar_t >{};
# endif
# ifdef AE_SIMD_SimdFloat16
	template <>				struct TMemCopyAvailable< SimdFloat16 >		: TMemCopyAvailable< SimdFloat16::Native_t >{};
	template <>				struct TZeroMemAvailable< SimdFloat16 >		: TZeroMemAvailable< SimdFloat16::Native_t >{};
	template <>				struct TUnwrap< SimdFloat16 >				: TUnwrap< SimdFloat16::Scalar_t >{};
# endif
# ifdef AE_SIMD_SimdDouble8
	template <>				struct TMemCopyAvailable< SimdDouble8 >		: TMemCopyAvailable< SimdDouble8::Native_t >{};
	template <>				struct TZeroMemAvailable< SimdDouble8 >		: TZeroMemAvailable< SimdDouble8::Native_t >{};
	template <>				struct TUnwrap< SimdDouble8 >				: TUnwrap< SimdDouble8::Scalar_t >{};
# endif
# ifdef AE_SIMD_SimdTInt512
	template <typename T>	struct TMemCopyAvailable< SimdTInt512<T> >	: TMemCopyAvailable< typename SimdTInt512<T>::Native_t >{};
	template <typename T>	struct TZeroMemAvailable< SimdTInt512<T> >	: TZeroMemAvailable< typename SimdTInt512<T>::Native_t >{};
	template <typename T>	struct TUnwrap< SimdTInt512<T> >			: TUnwrap< typename SimdTInt512<T>::Scalar_t >{};
# endif
# ifdef AE_SIMD_SimdHalf4
	template <>				struct TMemCopyAvailable< SimdHalf4 >		: TMemCopyAvailable< SimdHalf4::Native_t >{};
	template <>				struct TZeroMemAvailable< SimdHalf4 >		: TZeroMemAvailable< SimdHalf4::Native_t >{};
	template <>				struct TUnwrap< SimdHalf4 >					: TUnwrap< SimdHalf4::Scalar_t >{};
# endif
# ifdef AE_SIMD_SimdHalf8
	template <>				struct TMemCopyAvailable< SimdHalf8 >		: TMemCopyAvailable< SimdHalf8::Native_t >{};
	template <>				struct TZeroMemAvailable< SimdHalf8 >		: TZeroMemAvailable< SimdHalf8::Native_t >{};
	template <>				struct TUnwrap< SimdHalf8 >					: TUnwrap< SimdHalf8::Scalar_t >{};
# endif
# ifdef AE_SIMD_SimdTInt64
	template <typename T>	struct TMemCopyAvailable< SimdTInt64<T> >	: TMemCopyAvailable< typename SimdTInt64<T>::Native_t >{};
	template <typename T>	struct TZeroMemAvailable< SimdTInt64<T> >	: TZeroMemAvailable< typename SimdTInt64<T>::Native_t >{};
	template <typename T>	struct TUnwrap< SimdTInt64<T> >				: TUnwrap< typename SimdTInt64<T>::Scalar_t >{};
# endif

# ifdef AE_COMPILER_GCC
#	pragma GCC diagnostic pop
# endif

} // AE::Base
