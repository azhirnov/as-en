// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'
/*
	Static array of SIMD types.
	Can be used as SIMT or SoA SIMD.
*/

#pragma once

#include "base/SIMD/Math.h"

namespace AE::Base
{
namespace _hidden_
{

	//
	// SIMD Vector
	//

	template <typename ScalarType, typename SimdType, usize ScalarCount_v>
	class TSimdVector
	{
	// types
	public:
		using Self		= TSimdVector< ScalarType, SimdType, ScalarCount_v >;
		using Scalar_t	= ScalarType;
		using SIMD_t	= SimdType;

	private:
		static constexpr usize	_elemInSIMD = sizeof(SIMD_t) / sizeof(ScalarType);
		static constexpr usize	_arrSize	= (ScalarCount_v + _elemInSIMD-1) / _elemInSIMD;
		StaticAssert( _arrSize > 0 );


	// variables
	private:
		SIMD_t		_arr [_arrSize];


	// methods
	public:
		TSimdVector ()												__NE___	{}

		ND_ SIMD_t *			begin ()							__NE___	{ return &_arr[0]; }
		ND_ SIMD_t *			end ()								__NE___	{ return &_arr[Count()]; }

		ND_ SIMD_t const*		begin ()							C_NE___	{ return &_arr[0]; }
		ND_ SIMD_t const*		end ()								C_NE___	{ return &_arr[Count()]; }

		ND_ SIMD_t &			operator [] (usize i)				__NE___	{ ASSERT( i < Count() );  return _arr[i]; }
		ND_ SIMD_t const&		operator [] (usize i)				C_NE___	{ ASSERT( i < Count() );  return _arr[i]; }

	//	ND_ Scalar_t			GetScalar (usize i)					C_NE___	{ ASSERT( i < ScalarCount() );  return _arr[i%Count()].Get( i/Count() ); }
	//		void				SetScalar (usize i, Scalar_t value)	__NE___	{ ASSERT( i < ScalarCount() );  _arr[i%Count()].Set( i/Count(), value ); }

		NdCx__ static usize		ScalarCount ()						__NE___	{ return ScalarCount_v; }
		NdCx__ static usize		Count ()							__NE___	{ return _arrSize; }

	  #if AE_SIMD_AVX > 0
		NdCx__ static bool		IsAVX512 ()							__NE___	{ return sizeof(SIMD_t) == 64; }
		NdCx__ static bool		IsAVX256 ()							__NE___	{ return sizeof(SIMD_t) == 32; }
	  #endif
	  #if AE_SIMD_SSE > 0
		NdCx__ static bool		IsSSE ()							__NE___	{ return sizeof(SIMD_t) == 16; }
	  #endif
	  #if AE_SIMD_NEON
		NdCx__ static bool		IsNeon ()							__NE___	{ return sizeof(SIMD_t) >= 16; }
	  #endif
	};


	template <typename T, usize Count>
	struct SelectSimdInt
	{
		static constexpr usize	size = Count * sizeof(T);

	  #ifdef AE_SIMD_SimdTInt512
		using type	= Conditional< (size <= 128), SimdTInt128<T>, Conditional< (size <= 256), SimdTInt256<T>, SimdTInt512<T> >>;
	  #elif defined(AE_SIMD_SimdTInt256)
		using type	= Conditional< (size <= 128), SimdTInt128<T>, SimdTInt256<T> >;
	  #elif defined(AE_SIMD_SimdTInt128) and defined(AE_SIMD_SimdTInt64)
		using type	= Conditional< (size <= 64), SimdTInt64<T>, SimdTInt128<T> >;
	  #elif defined(AE_SIMD_SimdTInt128)
		using type	= SimdTInt128<T>;
	  #elif defined(AE_SIMD_SimdTInt64)
		using type	= SimdTInt64<T>;
	  #else
		using type	= T;
	  #endif
	};


	template <usize Count>
	struct SelectSimdFloat
	{
	  #ifdef AE_SIMD_SimdFloat16
		using type	= Conditional< (Count <= 4), SimdFloat4, Conditional< (Count <= 8), SimdFloat8, SimdFloat16 >>;
	  #elif defined(AE_SIMD_SimdFloat8)
		using type	= Conditional< (Count <= 4), SimdFloat4, SimdFloat8 >;
	  #elif defined(AE_SIMD_SimdFloat4)
		using type	= SimdFloat4;
	  #else
		using type	= float;
	  #endif
	};


	template <usize Count>
	struct SelectSimdDouble
	{
	  #ifdef AE_SIMD_SimdDouble8
		using type	= Conditional< (Count <= 2), SimdDouble2, Conditional< (Count <= 4), SimdDouble4, SimdDouble8 >>;
	  #elif defined(AE_SIMD_SimdDouble4)
		using type	= Conditional< (Count <= 2), SimdDouble2, SimdDouble4 >;
	  #elif defined(AE_SIMD_SimdDouble2)
		using type	= SimdDouble2;
	  #else
		using type	= double;
	  #endif
	};


	template <usize Count>
	struct SelectSimdHalf
	{
	  #ifdef AE_SIMD_SimdHalf32
		using type	= Conditional< (Count <= 8), SimdHalf8, Conditional< (Count <= 16), SimdHalf16, SimdHalf32 >>;
	  #elif defined(AE_SIMD_SimdHalf16)
		using type	= Conditional< (Count <= 8), SimdHalf8, SimdHalf16 >;
	  #elif defined(AE_SIMD_SimdHalf8) and defined(AE_SIMD_SimdHalf4)
		using type	= Conditional< (Count <= 4), SimdHalf4, SimdHalf8 >;
	  #elif defined(AE_SIMD_SimdHalf8)
		using type	= SimdHalf8;
	  #elif defined(AE_SIMD_SimdHalf4)
		using type	= SimdHalf4;
	  #else
		//using type	= half;		// only if have 'fphp' cpu feature
	  #endif
	};


	template <typename T, usize Count>
	struct SelectSIMDType
	{
		using type	=	typename
							Conditional< IsSame< T, float >,	SelectSimdFloat<Count>,
							Conditional< IsSame< T, double >,	SelectSimdDouble<Count>,
							Conditional< IsSame< T, half >,		SelectSimdHalf<Count>,
							Conditional< IsInteger<T>,			SelectSimdInt<T, Count>,
								void >>>>::type;
	};


	template <typename T, usize Count>
	struct SelectSimdVector
	{
		using simd_type		= typename Conditional< IsScalar<T>, SelectSIMDType<T,Count>, TypeToType<T> >::type;
		using scalar_type	= Conditional< IsScalar<T>, T, UnwrapType<simd_type> >;
		using type			= TSimdVector< scalar_type, simd_type, Count >;
	};

	template <typename T>
	struct SingleElement_SimdVector
	{
		StaticAssert( IsScalar<T> );
		using simd	= typename SelectSimdVector< T, 32 >::type::SIMD_t;
		static constexpr uint	count = sizeof(simd) / sizeof(T);
		using type	= typename SelectSimdVector< simd, count >::type;
		StaticAssert( type::Count() == 1 );
	};

} // _hidden_


	template <typename ScalarOrSIMD, usize ScalarCount>
	using SimdVector = typename Base::_hidden_::SelectSimdVector< ScalarOrSIMD, ScalarCount >::type;

	template <typename ScalarType>
	using SimdVector_SingleElement = typename Base::_hidden_::SingleElement_SimdVector< ScalarType >::type;

/*
=================================================
	Sum
=================================================
*/
	template <typename T, usize C>
	void  Sum (OUT SimdVector<T,C> &dst,
			   const SimdVector<T,C> &x, const SimdVector<T,C> &y) __NE___
	{
		for (usize i = 0; i < dst.Count(); ++i)
			dst[i] = x[i] + y[i];
	}

	template <typename T, usize C>
	void  Sum (OUT SimdVector<T,C> &dst,
			   const SimdVector<T,C> &x, const SimdVector<T,C> &y, const SimdVector<T,C> &z) __NE___
	{
		for (usize i = 0; i < dst.Count(); ++i)
			dst[i] = x[i] + y[i] + z[i];
	}

	template <typename T, usize C>
	void  Sum (OUT SimdVector<T,C> &dst,
			   const SimdVector<T,C> &x, const SimdVector<T,C> &y, const SimdVector<T,C> &z, const SimdVector<T,C> &w) __NE___
	{
		for (usize i = 0; i < dst.Count(); ++i)
			dst[i] = (x[i] + y[i]) + (z[i] + w[i]);
	}

/*
=================================================
	Area
=================================================
*/
	template <typename T, usize C>
	void  Area (OUT SimdVector<T,C> &dst,
				const SimdVector<T,C> &x, const SimdVector<T,C> &y) __NE___
	{
		for (usize i = 0; i < dst.Count(); ++i)
			dst[i] = x[i] * y[i];
	}

	template <typename T, usize C>
	void  Area (OUT SimdVector<T,C> &dst,
				const SimdVector<T,C> &x, const SimdVector<T,C> &y, const SimdVector<T,C> &z) __NE___
	{
		for (usize i = 0; i < dst.Count(); ++i)
			dst[i] = x[i] * y[i] * z[i];
	}

	template <typename T, usize C>
	void  Area (OUT SimdVector<T,C> &dst,
				const SimdVector<T,C> &x, const SimdVector<T,C> &y, const SimdVector<T,C> &z, const SimdVector<T,C> &w) __NE___
	{
		for (usize i = 0; i < dst.Count(); ++i)
			dst[i] = (x[i] * y[i]) * (z[i] * w[i]);
	}

/*
=================================================
	Min/Max
=================================================
*/
	template <typename T, usize C>
	void  Min (OUT SimdVector<T,C> &dst, const SimdVector<T,C> &a, const SimdVector<T,C> &b) __NE___
	{
		for (usize i = 0; i < dst.Count(); ++i)
			dst[i] = Min( a[i], b[i] );
	}

	template <typename T, usize C>
	void  Max (OUT SimdVector<T,C> &dst, const SimdVector<T,C> &a, const SimdVector<T,C> &b) __NE___
	{
		for (usize i = 0; i < dst.Count(); ++i)
			dst[i] = Max( a[i], b[i] );
	}

/*
=================================================
	Dot (vec2)
=================================================
*/
	template <typename T, usize C>
	void  Dot (OUT SimdVector<T,C> &dst,
			   const SimdVector<T,C> &aX, const SimdVector<T,C> &aY,
			   const SimdVector<T,C> &bX, const SimdVector<T,C> &bY) __NE___
	{
		StaticAssert( IsAnyFloatPoint< typename SimdVector<T,C>::Scalar_t >);

		for (usize i = 0; i < dst.Count(); ++i)
			dst[i] = (aX[i] * bX[i]) + (aY[i] * bY[i]);
	}

/*
=================================================
	Dot (vec3)
=================================================
*/
	template <typename T, usize C>
	void  Dot (OUT SimdVector<T,C> &dst,
			   const SimdVector<T,C> &aX, const SimdVector<T,C> &aY, const SimdVector<T,C> &aZ,
			   const SimdVector<T,C> &bX, const SimdVector<T,C> &bY, const SimdVector<T,C> &bZ) __NE___
	{
		StaticAssert( IsAnyFloatPoint< typename SimdVector<T,C>::Scalar_t >);

		using V			= SimdVector<T,C>;
		using SIMD_t	= typename V::SIMD_t;

		if constexpr( V::IsAVX256() and SIMD_t::Has_FusedMulAdd() )
		{
			for (usize i = 0; i < dst.Count(); ++i)
				dst[i] = FusedMulAdd( aX[i], bX[i],	FusedMulAdd( aY[i], bY[i], aZ[i] * bZ[i] ));
		}else
		if constexpr( V::IsNeon() and SIMD_t::Has_FusedMulAdd() )
		{
			V	temp;

			for (usize i = 0; i < dst.Count(); ++i)
				temp[i] = FusedMulAdd( aY[i], bY[i], aZ[i] * bZ[i] );

			for (usize i = 0; i < dst.Count(); ++i)
				dst[i] = FusedMulAdd( aX[i], bX[i], temp[i] );
		}else
		{
			for (usize i = 0; i < dst.Count(); ++i)
				dst[i] = (aX[i] * bX[i]) + (aY[i] * bY[i]) + (aZ[i] * bZ[i]);
		}
	}

/*
=================================================
	Cross (vec3)
=================================================
*/
	template <typename T, usize C>
	void  Cross (OUT SimdVector<T,C> &dstX, OUT SimdVector<T,C> &dstY, OUT SimdVector<T,C> &dstZ,
				 const SimdVector<T,C> &aX, const SimdVector<T,C> &aY, const SimdVector<T,C> &aZ,
				 const SimdVector<T,C> &bX, const SimdVector<T,C> &bY, const SimdVector<T,C> &bZ) __NE___
	{
		StaticAssert( IsAnyFloatPoint< typename SimdVector<T,C>::Scalar_t >);

		using SIMD_t	= typename SimdVector<T,C>::SIMD_t;

		if constexpr( SIMD_t::Has_FusedMulAdd() )
		{
			for (usize i = 0; i < dstX.Count(); ++i)
			{
				auto	tx = aY[i] * bZ[i];
				auto	ty = aZ[i] * bX[i];
				auto	tz = aX[i] * bY[i];

				dstX[i] = FusedNegMulAdd( aZ[i], bY[i], tx );
				dstY[i] = FusedNegMulAdd( aX[i], bZ[i], ty );
				dstZ[i] = FusedNegMulAdd( aY[i], bX[i], tz );
			}
		}
		else
		{
			for (usize i = 0; i < dstX.Count(); ++i)
			{
				auto	tx = aZ[i] * bY[i];
				auto	ty = aX[i] * bZ[i];
				auto	tz = aY[i] * bX[i];

				dstX[i] = aY[i] * bZ[i] - tx;
				dstY[i] = aZ[i] * bX[i] - ty;
				dstZ[i] = aX[i] * bY[i] - tz;
			}
		}
	}

} // AE::Base
