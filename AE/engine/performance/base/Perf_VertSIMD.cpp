// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	[results](https://github.com/azhirnov/as-en/blob/dev/AE/docs/papers/bench-cpu/VerticalSIMD.md)
*/

#include "Perf_Common.h"

#ifdef AE_SIMD_SimdFloat4
namespace
{
	static constexpr uint	c_Step	= 2;
	static constexpr uint	c_Step2 = 2*c_Step;


	using AVec	= SimdVector< SimdFloat4, 4*c_Step2 >;
	struct A2Vec
	{
		AVec	lhsX, lhsY, lhsZ;
		AVec	rhsX, rhsY, rhsZ;
		AVec	dstX;
	};

	struct A3Vec
	{
		AVec	lhsX, lhsY, lhsZ;
		AVec	rhsX, rhsY, rhsZ;
		AVec	dstX, dstY, dstZ;
	};


	using BVec	= StaticArray< SimdFloat4, 3*c_Step2 >;
	struct B2Vec
	{
		BVec	lhs, rhs;
		BVec	dst;
	};

	StaticAssert( sizeof(A2Vec) <= sizeof(B2Vec) );


#ifdef AE_SIMD_SimdFloat8
	using DVec	= SimdVector< SimdFloat8, 8*c_Step >;
	struct D2Vec
	{
		DVec	lhsX, lhsY, lhsZ;
		DVec	rhsX, rhsY, rhsZ;
		DVec	dstX;
	};

	struct D3Vec
	{
		DVec	lhsX, lhsY, lhsZ;
		DVec	rhsX, rhsY, rhsZ;
		DVec	dstX, dstY, dstZ;
	};

	StaticAssert( sizeof(A2Vec)*(2*c_Step)/c_Step2 == sizeof(D2Vec) );
	StaticAssert( sizeof(A3Vec)*(2*c_Step)/c_Step2 == sizeof(D3Vec) );
#endif


	static constexpr uint	c_MaxIter	= 10;
	static constexpr uint	c_Repeat	= 4;


	// vertical SIMD
	template <typename A>
	struct _AVec_BaseOp
	{
		MutableArrayView< A >	data;

		_AVec_BaseOp (void* ptr, usize count) : data{ Cast<A>(ptr), (count*3)/4 } {}

		void  Init ()
		{
			const float	count = float(data.size() * AVec::Count() * 6);
			for (usize i = 0; i < data.size(); ++i)
			{
				auto&	e = data[i];
				for (usize j = 0; j < AVec::Count(); ++j)
				{
					const usize	idx = (j + i*AVec::Count()) * 6;
					e.lhsX[j] = SimdFloat4{ float(idx+0)/count };
					e.lhsY[j] = SimdFloat4{ float(idx+1)/count };
					e.lhsZ[j] = SimdFloat4{ float(idx+2)/count };
					e.rhsX[j] = SimdFloat4{ float(idx+3)/count };
					e.rhsY[j] = SimdFloat4{ float(idx+4)/count };
					e.rhsZ[j] = SimdFloat4{ float(idx+5)/count };
				}
			}
		}

		ND_ HashVal  Hash () const
		{
			HashVal	h;
			for (usize i = 0; i < data.size(); ++i)
			{
				for (usize j = 0; j < AVec::Count(); ++j)
				{
					h += HashOf( &data[i].dstX[j], sizeof(data[i].dstX[j]) );
					if constexpr( IsSame< A, A3Vec >) {
						h += HashOf( &data[i].dstY[j], sizeof(data[i].dstY[j]) );
						h += HashOf( &data[i].dstZ[j], sizeof(data[i].dstZ[j]) );
					}
				}
			}
			return h;
		}

		ND_ Bytes	Size ()			const	{ return data.DataSize(); }
		ND_ usize	VecCount ()		const	{ return data.size() * AVec::ScalarCount(); }

		ND_ usize	DotFlops ()		const	{ return data.size() * AVec::ScalarCount() * (3 + 2) * c_Repeat; }
		ND_ usize	CrossFlops ()	const	{ return data.size() * AVec::ScalarCount() * (6 + 3) * c_Repeat; }
	};
	using A2Vec_BaseOp = _AVec_BaseOp< A2Vec >;		// Dot
	using A3Vec_BaseOp = _AVec_BaseOp< A3Vec >;		// Cross


	// horizontal SIMD
	struct B2Vec_BaseOp
	{
		MutableArrayView< B2Vec >	data;

		B2Vec_BaseOp (void* ptr, usize count) : data{ Cast<B2Vec>(ptr), count } {}

		void  Init ()
		{
			const float	count = float(data.size() * data[0].lhs.size() * 2);
			for (usize i = 0; i < data.size(); ++i)
			{
				auto&	e = data[i];
				for (usize j = 0; j < e.lhs.size(); ++j)
				{
					const usize	idx = (j + i*e.lhs.size()) * 2;
					e.lhs[j] = SimdFloat4{ float(idx+0)/count };
					e.rhs[j] = SimdFloat4{ float(idx+1)/count };
				}
			}
		}

		ND_ HashVal  Hash () const
		{
			HashVal	h;
			for (usize i = 0; i < data.size(); ++i)
			{
				for (usize j = 0; j < data[i].dst.size(); ++j)
				{
					h += HashOf( &data[i].dst[j], sizeof(data[i].dst[j]) );
				}
			}
			return h;
		}

		ND_ Bytes	Size ()			const	{ return data.DataSize(); }
		ND_ usize	VecCount ()		const	{ return data.size() * BVec{}.size(); }

		ND_ usize	DotFlops ()		const	{ return VecCount() * (3+2) * c_Repeat; }
		ND_ usize	CrossFlops ()	const	{ return VecCount() * 3 * (1+2) * c_Repeat; }
	};


#ifdef AE_SIMD_SimdFloat8

	// vertical SIMD (AVX)
	template <typename D>
	struct _DVec_BaseOp
	{
		MutableArrayView< D >	data;

		_DVec_BaseOp (void* ptr, usize count) : data{ Cast<D>(ptr), (count*sizeof(AVec)*3)/(sizeof(DVec)*4) } {}

		void  Init ()
		{
			const float	count = float(data.size() * DVec::Count() * 6);
			for (usize i = 0; i < data.size(); ++i)
			{
				auto&	e = data[i];
				for (usize j = 0; j < DVec::Count(); ++j)
				{
					const usize	idx = (j + i*DVec::Count()) * 6;
					e.lhsX[j] = SimdFloat8{ float(idx+0)/count };
					e.lhsY[j] = SimdFloat8{ float(idx+1)/count };
					e.lhsZ[j] = SimdFloat8{ float(idx+2)/count };
					e.rhsX[j] = SimdFloat8{ float(idx+3)/count };
					e.rhsY[j] = SimdFloat8{ float(idx+4)/count };
					e.rhsZ[j] = SimdFloat8{ float(idx+5)/count };
				}
			}
		}

		ND_ HashVal  Hash () const
		{
			HashVal	h;
			for (usize i = 0; i < data.size(); ++i)
			{
				for (usize j = 0; j < DVec::Count(); ++j)
				{
					h += HashOf( &data[i].dstX[j], sizeof(data[i].dstX[j]) );
					if constexpr( IsSame< D, D3Vec >) {
						h += HashOf( &data[i].dstY[j], sizeof(data[i].dstY[j]) );
						h += HashOf( &data[i].dstZ[j], sizeof(data[i].dstZ[j]) );
					}
				}
			}
			return h;
		}

		ND_ Bytes	Size ()			const	{ return data.DataSize(); }
		ND_ usize	VecCount ()		const	{ return data.size() * DVec::ScalarCount(); }

		ND_ usize	DotFlops ()		const	{ return data.size() * DVec::ScalarCount() * (3 + 2) * c_Repeat; }
		ND_ usize	CrossFlops ()	const	{ return data.size() * DVec::ScalarCount() * (6 + 3) * c_Repeat; }
	};
	using D2Vec_BaseOp = _DVec_BaseOp< D2Vec >;		// Dot
	using D3Vec_BaseOp = _DVec_BaseOp< D3Vec >;		// Cross
#endif


#if AE_SIMD_SSE >= 20
	static void  Dot3_SSE (OUT BVec &dst, const BVec &lhs, const BVec &rhs)
	{
		for (usize i = 0; i < dst.size(); ++i)
		{
			__m128	a = _mm_mul_ps( lhs[i].Ref(), rhs[i].Ref() );
			__m128	s = _mm_shuffle_ps( a, a, _MM_SHUFFLE( 2, 3, 0, 1 ));	// y, x, w, z
					a = _mm_add_ps( a, s );									// x+y, y+x, z+w, w+z
					s = _mm_shuffle_ps( a, a, _MM_SHUFFLE( 0, 1, 2, 3 ));	// w+z, z+w, y+x, x+y
			dst[i] = SimdFloat4{_mm_add_ps( a, s )};						// x+y+w+z, y+x+z+w, z+w+y+x, w+z+x+y
		}
	}

	struct Dot3_SSE_Op : B2Vec_BaseOp
	{
		Dot3_SSE_Op (void* ptr, usize count) : B2Vec_BaseOp{ ptr, count } {}

		ND_ usize	Flops () const	{ return DotFlops(); }

		void  Run () {
			for (auto& e : data) {
				Dot3_SSE( OUT e.dst, e.lhs, e.rhs );
				if constexpr( c_Repeat > 1 ) {
					Dot3_SSE( OUT e.rhs, e.dst, e.lhs );
				}
				if constexpr( c_Repeat > 2 ) {
					Dot3_SSE( OUT e.lhs, e.rhs, e.dst );
					Dot3_SSE( OUT e.dst, e.rhs, e.lhs );
				}
				if constexpr( c_Repeat > 4 ) {
					Dot3_SSE( OUT e.rhs, e.dst, e.lhs );
					Dot3_SSE( OUT e.dst, e.lhs, e.rhs );
				}
			}
		}
	};
#endif

#if AE_SIMD_SSE >= 30
	static void  Dot3_SSE30 (OUT BVec &dst, const BVec &lhs, const BVec &rhs)
	{
		for (usize i = 0; i < dst.size(); ++i)
		{
			__m128	a = _mm_mul_ps( lhs[i].Ref(), rhs[i].Ref() );
					a = _mm_hadd_ps( a, a );
			dst[i] = SimdFloat4{_mm_hadd_ps( a, a )};
		}
	}

	struct Dot3_SSE30_Op : B2Vec_BaseOp
	{
		Dot3_SSE30_Op (void* ptr, usize count) : B2Vec_BaseOp{ ptr, count } {}

		ND_ usize	Flops () const	{ return DotFlops(); }

		void  Run () {
			for (auto& e : data) {
				Dot3_SSE30( OUT e.dst, e.lhs, e.rhs );
				if constexpr( c_Repeat > 1 ) {
					Dot3_SSE30( OUT e.rhs, e.dst, e.lhs );
				}
				if constexpr( c_Repeat > 2 ) {
					Dot3_SSE30( OUT e.lhs, e.rhs, e.dst );
					Dot3_SSE30( OUT e.dst, e.rhs, e.lhs );
				}
				if constexpr( c_Repeat > 4 ) {
					Dot3_SSE30( OUT e.rhs, e.dst, e.lhs );
					Dot3_SSE30( OUT e.dst, e.lhs, e.rhs );
				}
			}
		}
	};
#endif

#if AE_SIMD_SSE >= 41
	static void  Dot3_SSE41 (OUT BVec &dst, const BVec &lhs, const BVec &rhs)
	{
		for (usize i = 0; i < dst.size(); ++i)
		{
			dst[i] = SimdFloat4{_mm_dp_ps( lhs[i].Ref(), rhs[i].Ref(), 0x7F )};
		}
	}

	struct Dot3_SSE41_Op : B2Vec_BaseOp
	{
		Dot3_SSE41_Op (void* ptr, usize count) : B2Vec_BaseOp{ ptr, count } {}

		ND_ usize	Flops () const	{ return DotFlops(); }

		void  Run () {
			for (auto& e : data) {
				Dot3_SSE41( OUT e.dst, e.lhs, e.rhs );
				if constexpr( c_Repeat > 1 ) {
					Dot3_SSE41( OUT e.rhs, e.dst, e.lhs );
				}
				if constexpr( c_Repeat > 2 ) {
					Dot3_SSE41( OUT e.lhs, e.rhs, e.dst );
					Dot3_SSE41( OUT e.dst, e.rhs, e.lhs );
				}
				if constexpr( c_Repeat > 4 ) {
					Dot3_SSE41( OUT e.rhs, e.dst, e.lhs );
					Dot3_SSE41( OUT e.dst, e.lhs, e.rhs );
				}
			}
		}
	};
#endif

#if AE_SIMD_NEON
	static void  Dot3_Neon_v1 (OUT BVec &dst, const BVec &lhs, const BVec &rhs)
	{
		for (usize i = 0; i < dst.size(); ++i)
		{
			float32x4_t	a = lhs[i].Ref();
			float32x4_t	b = rhs[i].Ref();
			float32x4_t	m = vmulq_f32( a, b );							// a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w
						m = vmlaq_f32( m,	vextq_f32( a, a, 1 ),	// a{y, z, w, x}
											vextq_f32( b, b, 1 )	// b{y, z, w, x}
										);								// a.y*b.y + a.x*b.x,  a.z*b.z + a.y*b.y,  a.w*a.w + a.z*b.z,  a.x*b.x + a.w*b.w
						m = vmlaq_f32( m,	vextq_f32( a, a, 2 ),	// a{z, w, x, y}
											vextq_f32( b, b, 2 )	// b{z, w, x, y}
										);								// {a.z*b.z, a.w*b.w, a.x*b.x, a.y*b.y} + m
			dst[i] = SimdFloat4{ m };	// a.z*b.z + a.y*b.y + a.x*b.x,  a.w*b.w + a.z*b.z + a.y*b.y,  a.x*b.x + a.w*a.w + a.z*b.z,  a.y*b.y + a.x*b.x + a.w*b.w
										// ab{z+y+x}, ab{w+z+y}, ab{x+w+z}, ab{y+x+w}
		}
	}

	static void  Dot3_Neon_v2 (OUT BVec &dst, const BVec &lhs, const BVec &rhs)
	{
		for (usize i = 0; i < dst.size(); ++i)
		{
			float32x4_t	a = lhs[i].Ref();
			float32x4_t	b = rhs[i].Ref();
			float32x4_t m = vmulq_f32( a, b );		// a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w
			float32x2_t v1 = vget_low_f32( m );		// a.x*b.x, a.y*b.y
			float32x2_t v2 = vget_high_f32( m );	// a.z*b.z, a.w*b.w
			v1 = vpadd_f32( v1, v1 );				// a.x*b.x + a.y*b.y,  ???
			v2 = vdup_lane_f32( v2, 0 );			// a.z*b.z, a.z*b.z
			v1 = vadd_f32( v1, v2 );				// a.x*b.x + a.y*b.y + a.z*b.z, ??? + a.z*b.z
			dst[i] = SimdFloat4{ vcombine_f32( v1, v1 )};
		}
	}

	struct Dot3_Neon1_Op : B2Vec_BaseOp
	{
		Dot3_Neon1_Op (void* ptr, usize count) : B2Vec_BaseOp{ ptr, count } {}

		ND_ usize	Flops () const	{ return DotFlops(); }

		void  Run () {
			for (auto& e : data) {
				Dot3_Neon_v1( OUT e.dst, e.lhs, e.rhs );
				if constexpr( c_Repeat > 1 ) {
					Dot3_Neon_v1( OUT e.rhs, e.dst, e.lhs );
				}
				if constexpr( c_Repeat > 2 ) {
					Dot3_Neon_v1( OUT e.lhs, e.rhs, e.dst );
					Dot3_Neon_v1( OUT e.dst, e.rhs, e.lhs );
				}
				if constexpr( c_Repeat > 4 ) {
					Dot3_Neon_v1( OUT e.rhs, e.dst, e.lhs );
					Dot3_Neon_v1( OUT e.dst, e.lhs, e.rhs );
				}
			}
		}
	};

	struct Dot3_Neon2_Op : B2Vec_BaseOp
	{
		Dot3_Neon2_Op (void* ptr, usize count) : B2Vec_BaseOp{ ptr, count } {}

		ND_ usize	Flops () const	{ return DotFlops(); }

		void  Run () {
			for (auto& e : data) {
				Dot3_Neon_v2( OUT e.dst, e.lhs, e.rhs );
				if constexpr( c_Repeat > 1 ) {
					Dot3_Neon_v2( OUT e.rhs, e.dst, e.lhs );
				}
				if constexpr( c_Repeat > 2 ) {
					Dot3_Neon_v2( OUT e.lhs, e.rhs, e.dst );
					Dot3_Neon_v2( OUT e.dst, e.rhs, e.lhs );
				}
				if constexpr( c_Repeat > 4 ) {
					Dot3_Neon_v2( OUT e.rhs, e.dst, e.lhs );
					Dot3_Neon_v2( OUT e.dst, e.lhs, e.rhs );
				}
			}
		}
	};
#endif

#if AE_SIMD_FMA
	template <typename VecT>
	static void  Dot_v1 (OUT VecT &dst,
						 const VecT &lhsX, const VecT &lhsY, const VecT &lhsZ,
						 const VecT &rhsX, const VecT &rhsY, const VecT &rhsZ)
	{
		for (usize i = 0; i < dst.Count(); ++i)
			dst[i] = FusedMulAdd( lhsX[i], rhsX[i],	FusedMulAdd( lhsY[i], rhsY[i], (lhsZ[i] * rhsZ[i]) ));
	}
#endif

	template <typename VecT>
	static void  Dot_v2 (OUT VecT &dst,
						 const VecT &lhsX, const VecT &lhsY, const VecT &lhsZ,
						 const VecT &rhsX, const VecT &rhsY, const VecT &rhsZ)
	{
		for (usize i = 0; i < dst.Count(); ++i)
			dst[i] = (lhsX[i] * rhsX[i]) + (lhsY[i] * rhsY[i]) + (lhsZ[i] * rhsZ[i]);
	}

	template <typename VecT>
	static void  Dot_v3 (OUT VecT &dst,
						 const VecT &lhsX, const VecT &lhsY, const VecT &lhsZ,
						 const VecT &rhsX, const VecT &rhsY, const VecT &rhsZ)
	{
		VecT	temp_x, temp_y, temp_z;
		for (usize i = 0; i < dst.Count(); ++i)		temp_x[i] = lhsX[i] * rhsX[i];
		for (usize i = 0; i < dst.Count(); ++i)		temp_y[i] = lhsY[i] * rhsY[i];
		for (usize i = 0; i < dst.Count(); ++i)		temp_z[i] = lhsZ[i] * rhsZ[i];
		for (usize i = 0; i < dst.Count(); ++i)		dst[i] = temp_x[i] + temp_y[i];
		for (usize i = 0; i < dst.Count(); ++i)		dst[i] = dst[i] + temp_z[i];
	}

	template <typename VecT>
	static void  Dot_v4 (OUT VecT &dst,
						 const VecT &lhsX, const VecT &lhsY, const VecT &lhsZ,
						 const VecT &rhsX, const VecT &rhsY, const VecT &rhsZ)
	{
		VecT	temp_x, temp_y, temp_z;
		for (usize i = 0; i < dst.Count(); ++i)		temp_x[i] = lhsX[i] * rhsX[i];
		for (usize i = 0; i < dst.Count(); ++i)		temp_y[i] = lhsY[i] * rhsY[i];
		for (usize i = 0; i < dst.Count(); ++i)		temp_z[i] = lhsZ[i] * rhsZ[i];
		for (usize i = 0; i < dst.Count(); ++i)		dst[i] = temp_x[i] + temp_y[i] + temp_z[i];
	}

#if AE_SIMD_FMA
	template <typename VecT>
	static void  Dot_v5 (OUT VecT &dst,
						 const VecT &lhsX, const VecT &lhsY, const VecT &lhsZ,
						 const VecT &rhsX, const VecT &rhsY, const VecT &rhsZ)
	{
		VecT	temp;

		for (usize i = 0; i < dst.Count(); ++i)
			temp[i] = FusedMulAdd( lhsY[i], rhsY[i], lhsZ[i] * rhsZ[i] );

		for (usize i = 0; i < dst.Count(); ++i)
			dst[i] = FusedMulAdd( lhsX[i], rhsX[i],	temp[i] );
	}
#endif


#if AE_SIMD_FMA
	struct DotV1_Op : A2Vec_BaseOp
	{
		DotV1_Op (void* ptr, usize count) : A2Vec_BaseOp{ ptr, count } {}

		ND_ usize	Flops () const	{ return DotFlops(); }

		void  Run () {
			for (auto& e : data) {
				Dot_v1( OUT e.dstX,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
				if constexpr( c_Repeat > 1 ) {
					Dot_v1( OUT e.rhsX,  e.rhsZ, e.lhsX, e.rhsY,  e.lhsY, e.dstX, e.lhsZ );
				}
				if constexpr( c_Repeat > 2 ) {
					Dot_v1( OUT e.dstX,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
					Dot_v1( OUT e.rhsX,  e.rhsZ, e.lhsX, e.rhsY,  e.lhsY, e.dstX, e.lhsZ );
				}
				if constexpr( c_Repeat > 4 ) {
					Dot_v1( OUT e.rhsX,  e.rhsZ, e.lhsX, e.rhsY,  e.lhsY, e.dstX, e.lhsZ );
					Dot_v1( OUT e.dstX,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
				}
			}
		}
	};
#endif

	struct DotV2_Op : A2Vec_BaseOp
	{
		DotV2_Op (void* ptr, usize count) : A2Vec_BaseOp{ ptr, count } {}

		ND_ usize	Flops () const	{ return DotFlops(); }

		void  Run () {
			for (auto& e : data) {
				Dot_v2( OUT e.dstX,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
				if constexpr( c_Repeat > 1 ) {
					Dot_v2( OUT e.rhsX,  e.rhsZ, e.lhsX, e.rhsY,  e.lhsY, e.dstX, e.lhsZ );
				}
				if constexpr( c_Repeat > 2 ) {
					Dot_v2( OUT e.dstX,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
					Dot_v2( OUT e.rhsX,  e.rhsZ, e.lhsX, e.rhsY,  e.lhsY, e.dstX, e.lhsZ );
				}
				if constexpr( c_Repeat > 4 ) {
					Dot_v2( OUT e.rhsX,  e.rhsZ, e.lhsX, e.rhsY,  e.lhsY, e.dstX, e.lhsZ );
					Dot_v2( OUT e.dstX,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
				}
			}
		}
	};

	struct DotV3_Op : A2Vec_BaseOp
	{
		DotV3_Op (void* ptr, usize count) : A2Vec_BaseOp{ ptr, count } {}

		ND_ usize	Flops () const	{ return DotFlops(); }

		void  Run () {
			for (auto& e : data) {
				Dot_v3( OUT e.dstX,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
				if constexpr( c_Repeat > 1 ) {
					Dot_v3( OUT e.rhsX,  e.rhsZ, e.lhsX, e.rhsY,  e.lhsY, e.dstX, e.lhsZ );
				}
				if constexpr( c_Repeat > 2 ) {
					Dot_v3( OUT e.dstX,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
					Dot_v3( OUT e.rhsX,  e.rhsZ, e.lhsX, e.rhsY,  e.lhsY, e.dstX, e.lhsZ );
				}
				if constexpr( c_Repeat > 4 ) {
					Dot_v3( OUT e.rhsX,  e.rhsZ, e.lhsX, e.rhsY,  e.lhsY, e.dstX, e.lhsZ );
					Dot_v3( OUT e.dstX,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
				}
			}
		}
	};

	struct DotV4_Op : A2Vec_BaseOp
	{
		DotV4_Op (void* ptr, usize count) : A2Vec_BaseOp{ ptr, count } {}

		ND_ usize	Flops () const	{ return DotFlops(); }

		void  Run () {
			for (auto& e : data) {
				Dot_v4( OUT e.dstX,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
				if constexpr( c_Repeat > 1 ) {
					Dot_v4( OUT e.rhsX,  e.rhsZ, e.lhsX, e.rhsY,  e.lhsY, e.dstX, e.lhsZ );
				}
				if constexpr( c_Repeat > 2 ) {
					Dot_v4( OUT e.dstX,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
					Dot_v4( OUT e.rhsX,  e.rhsZ, e.lhsX, e.rhsY,  e.lhsY, e.dstX, e.lhsZ );
				}
				if constexpr( c_Repeat > 4 ) {
					Dot_v4( OUT e.rhsX,  e.rhsZ, e.lhsX, e.rhsY,  e.lhsY, e.dstX, e.lhsZ );
					Dot_v4( OUT e.dstX,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
				}
			}
		}
	};

#if AE_SIMD_FMA
	struct DotV5_Op : A2Vec_BaseOp
	{
		DotV5_Op (void* ptr, usize count) : A2Vec_BaseOp{ ptr, count } {}

		ND_ usize	Flops () const	{ return DotFlops(); }

		void  Run () {
			for (auto& e : data) {
				Dot_v5( OUT e.dstX,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
				if constexpr( c_Repeat > 1 ) {
					Dot_v5( OUT e.rhsX,  e.rhsZ, e.lhsX, e.rhsY,  e.lhsY, e.dstX, e.lhsZ );
				}
				if constexpr( c_Repeat > 2 ) {
					Dot_v5( OUT e.dstX,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
					Dot_v5( OUT e.rhsX,  e.rhsZ, e.lhsX, e.rhsY,  e.lhsY, e.dstX, e.lhsZ );
				}
				if constexpr( c_Repeat > 4 ) {
					Dot_v5( OUT e.rhsX,  e.rhsZ, e.lhsX, e.rhsY,  e.lhsY, e.dstX, e.lhsZ );
					Dot_v5( OUT e.dstX,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
				}
			}
		}
	};
#endif


#ifdef AE_SIMD_SimdFloat8
# if AE_SIMD_FMA
	struct DotW1_Op : D2Vec_BaseOp
	{
		DotW1_Op (void* ptr, usize count) : D2Vec_BaseOp{ ptr, count } {}

		ND_ usize	Flops () const	{ return DotFlops(); }

		void  Run () {
			for (auto& e : data) {
				Dot_v1( OUT e.dstX,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
				if constexpr( c_Repeat > 1 ) {
					Dot_v1( OUT e.rhsX,  e.rhsZ, e.lhsX, e.rhsY,  e.lhsY, e.dstX, e.lhsZ );
				}
				if constexpr( c_Repeat > 2 ) {
					Dot_v1( OUT e.dstX,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
					Dot_v1( OUT e.rhsX,  e.rhsZ, e.lhsX, e.rhsY,  e.lhsY, e.dstX, e.lhsZ );
				}
				if constexpr( c_Repeat > 4 ) {
					Dot_v1( OUT e.rhsX,  e.rhsZ, e.lhsX, e.rhsY,  e.lhsY, e.dstX, e.lhsZ );
					Dot_v1( OUT e.dstX,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
				}
			}
		}
	};
# endif

	struct DotW2_Op : D2Vec_BaseOp
	{
		DotW2_Op (void* ptr, usize count) : D2Vec_BaseOp{ ptr, count } {}

		ND_ usize	Flops () const	{ return DotFlops(); }

		void  Run () {
			for (auto& e : data) {
				Dot_v2( OUT e.dstX,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
				if constexpr( c_Repeat > 1 ) {
					Dot_v2( OUT e.rhsX,  e.rhsZ, e.lhsX, e.rhsY,  e.lhsY, e.dstX, e.lhsZ );
				}
				if constexpr( c_Repeat > 2 ) {
					Dot_v2( OUT e.dstX,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
					Dot_v2( OUT e.rhsX,  e.rhsZ, e.lhsX, e.rhsY,  e.lhsY, e.dstX, e.lhsZ );
				}
				if constexpr( c_Repeat > 4 ) {
					Dot_v2( OUT e.rhsX,  e.rhsZ, e.lhsX, e.rhsY,  e.lhsY, e.dstX, e.lhsZ );
					Dot_v2( OUT e.dstX,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
				}
			}
		}
	};

	struct DotW3_Op : D2Vec_BaseOp
	{
		DotW3_Op (void* ptr, usize count) : D2Vec_BaseOp{ ptr, count } {}

		ND_ usize	Flops () const	{ return DotFlops(); }

		void  Run () {
			for (auto& e : data) {
				Dot_v3( OUT e.dstX,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
				if constexpr( c_Repeat > 1 ) {
					Dot_v3( OUT e.rhsX,  e.rhsZ, e.lhsX, e.rhsY,  e.lhsY, e.dstX, e.lhsZ );
				}
				if constexpr( c_Repeat > 2 ) {
					Dot_v3( OUT e.dstX,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
					Dot_v3( OUT e.rhsX,  e.rhsZ, e.lhsX, e.rhsY,  e.lhsY, e.dstX, e.lhsZ );
				}
				if constexpr( c_Repeat > 4 ) {
					Dot_v3( OUT e.rhsX,  e.rhsZ, e.lhsX, e.rhsY,  e.lhsY, e.dstX, e.lhsZ );
					Dot_v3( OUT e.dstX,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
				}
			}
		}
	};

	struct DotW4_Op : D2Vec_BaseOp
	{
		DotW4_Op (void* ptr, usize count) : D2Vec_BaseOp{ ptr, count } {}

		ND_ usize	Flops () const	{ return DotFlops(); }

		void  Run () {
			for (auto& e : data) {
				Dot_v4( OUT e.dstX,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
				Dot_v4( OUT e.rhsX,  e.rhsZ, e.lhsX, e.rhsY,  e.lhsY, e.dstX, e.lhsZ );
				if constexpr( c_Repeat > 2 ) {
					Dot_v4( OUT e.dstX,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
					Dot_v4( OUT e.rhsX,  e.rhsZ, e.lhsX, e.rhsY,  e.lhsY, e.dstX, e.lhsZ );
				}
				if constexpr( c_Repeat > 4 ) {
					Dot_v4( OUT e.rhsX,  e.rhsZ, e.lhsX, e.rhsY,  e.lhsY, e.dstX, e.lhsZ );
					Dot_v4( OUT e.dstX,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
				}
			}
		}
	};

# if AE_SIMD_FMA
	struct DotW5_Op : D2Vec_BaseOp
	{
		DotW5_Op (void* ptr, usize count) : D2Vec_BaseOp{ ptr, count } {}

		ND_ usize	Flops () const	{ return DotFlops(); }

		void  Run () {
			for (auto& e : data) {
				Dot_v5( OUT e.dstX,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
				if constexpr( c_Repeat > 1 ) {
					Dot_v5( OUT e.rhsX,  e.rhsZ, e.lhsX, e.rhsY,  e.lhsY, e.dstX, e.lhsZ );
				}
				if constexpr( c_Repeat > 2 ) {
					Dot_v5( OUT e.dstX,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
					Dot_v5( OUT e.rhsX,  e.rhsZ, e.lhsX, e.rhsY,  e.lhsY, e.dstX, e.lhsZ );
				}
				if constexpr( c_Repeat > 4 ) {
					Dot_v5( OUT e.rhsX,  e.rhsZ, e.lhsX, e.rhsY,  e.lhsY, e.dstX, e.lhsZ );
					Dot_v5( OUT e.dstX,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
				}
			}
		}
	};
# endif
#endif
//-----------------------------------------------------------------------------


#if AE_SIMD_SSE >= 20
	static void  Cross_SSE_v1 (OUT BVec &dst, const BVec &lhs, const BVec &rhs)
	{
		for (usize i = 0; i < dst.size(); ++i)
		{
			__m128 a = _mm_shuffle_ps( lhs[i].Ref(), lhs[i].Ref(), _MM_SHUFFLE( 3, 0, 2, 1 ));	// y, z, x, w
			__m128 b = _mm_shuffle_ps( rhs[i].Ref(), rhs[i].Ref(), _MM_SHUFFLE( 3, 0, 2, 1 ));	// y, z, x, w
			__m128 c = _mm_sub_ps(	_mm_mul_ps( lhs[i].Ref(), b ),								// l.x*r.y, l.y*r.z, l.z*r.x, l.w*r.w
									_mm_mul_ps( a, rhs[i].Ref() )								// l.y*r.x, l.z*r.y, l.x*r.z, l.w*r.w
								 );																// l.x*r.y - l.y*r.x, l.y*r.z - l.z*r.y, l.z*r.x - l.x*r.z, 0
			dst[i] = SimdFloat4{_mm_shuffle_ps( c, c, _MM_SHUFFLE( 3, 0, 2, 1 ))};				// l.y*r.z - l.z*r.y, l.z*r.x - l.x*r.z, l.x*r.y - l.y*r.x, 0
		}
	}

	static void  Cross_SSE_v2 (OUT BVec &dst, const BVec &lhs, const BVec &rhs)
	{
		for (usize i = 0; i < dst.size(); ++i)
		{
			__m128	a0 = _mm_shuffle_ps( lhs[i].Ref(), lhs[i].Ref(), _MM_SHUFFLE( 3, 0, 2, 1 ));	// y, z, x, w
			__m128	a1 = _mm_shuffle_ps( lhs[i].Ref(), lhs[i].Ref(), _MM_SHUFFLE( 3, 1, 0, 2 ));	// z, x, y, w
			__m128	b0 = _mm_shuffle_ps( rhs[i].Ref(), rhs[i].Ref(), _MM_SHUFFLE( 3, 0, 2, 1 ));	// y, z, x, w
			__m128	b1 = _mm_shuffle_ps( rhs[i].Ref(), rhs[i].Ref(), _MM_SHUFFLE( 3, 1, 0, 2 ));	// z, x, y, w
			__m128	a0b1 = _mm_mul_ps( a0, b1 );			// l.y*r.z, l.z*r.x, l.x*r.y, l.w*r.w
			__m128	a1b0 = _mm_mul_ps( a1, b0 );			// l.z*r.y, l.x*r.z, l.y*r.x, l.w*r.w
			dst[i] = SimdFloat4{_mm_sub_ps( a0b1, a1b0 )};	// l.y*r.z - l.z*r.y, l.z*r.x - l.x*r.z, l.x*r.y - l.y*r.x, 0
		}
	}

	struct Cross_SSEv1_Op : B2Vec_BaseOp
	{
		Cross_SSEv1_Op (void* ptr, usize count) : B2Vec_BaseOp{ ptr, count } {}

		ND_ usize	Flops () const	{ return CrossFlops(); }

		void  Run () {
			for (auto& e : data) {
				Cross_SSE_v1( OUT e.dst, e.lhs, e.rhs );
				if constexpr( c_Repeat > 1 ) {
					Cross_SSE_v1( OUT e.rhs, e.dst, e.lhs );
				}
				if constexpr( c_Repeat > 2 ) {
					Cross_SSE_v1( OUT e.lhs, e.rhs, e.dst );
					Cross_SSE_v1( OUT e.dst, e.rhs, e.lhs );
				}
				if constexpr( c_Repeat > 4 ) {
					Cross_SSE_v1( OUT e.rhs, e.dst, e.lhs );
					Cross_SSE_v1( OUT e.dst, e.lhs, e.rhs );
				}
			}
		}
	};

	struct Cross_SSEv2_Op : B2Vec_BaseOp
	{
		Cross_SSEv2_Op (void* ptr, usize count) : B2Vec_BaseOp{ ptr, count } {}

		ND_ usize	Flops () const	{ return CrossFlops(); }

		void  Run () {
			for (auto& e : data) {
				Cross_SSE_v2( OUT e.dst, e.lhs, e.rhs );
				if constexpr( c_Repeat > 1 ) {
					Cross_SSE_v2( OUT e.rhs, e.dst, e.lhs );
				}
				if constexpr( c_Repeat > 2 ) {
					Cross_SSE_v2( OUT e.lhs, e.rhs, e.dst );
					Cross_SSE_v2( OUT e.dst, e.rhs, e.lhs );
				}
				if constexpr( c_Repeat > 4 ) {
					Cross_SSE_v2( OUT e.rhs, e.dst, e.lhs );
					Cross_SSE_v2( OUT e.dst, e.lhs, e.rhs );
				}
			}
		}
	};
#endif

#if AE_SIMD_NEON
	static void  Cross_Neon_v1 (OUT BVec &dst, const BVec &lhs, const BVec &rhs)
	{
		for (usize i = 0; i < dst.size(); ++i)
		{
			float32x4_t	a = lhs[i].Ref();
			float32x4_t	b = rhs[i].Ref();

			float32x4_t	s0 = __builtin_shufflevector( a, a, 1, 2, 0, 3 );	// a{y, z, x, w}
			float32x4_t	s1 = __builtin_shufflevector( b, b, 2, 0, 1, 3 );	// b{z, x, y, w}
			float32x4_t	m0 = vmulq_f32( s0, b );							// a.y*b.x, a.z*b.y, a.x*b.z, a.w*b.w
			float32x4_t	m1 = vmulq_f32( s0, s1 );							// a.y*b.z, a.z*b.x, a.x*b.y, a.w*b.w
			float32x4_t	s2 = __builtin_shufflevector( m0, m0, 1, 2, 0, 3 );	// m0{y, z, x, w} = a.z*b.y, a.x*b.z, a.y*b.x, a.w*b.w

			dst[i] = SimdFloat4{ vsubq_f32( m1, s2 )};						// a.y*b.z - a.z*b.y,  a.z*b.x - a.x*b.z,  a.x*b.y - a.y*b.x,  0
		}
	}

	struct Cross_Neon1_Op : B2Vec_BaseOp
	{
		Cross_Neon1_Op (void* ptr, usize count) : B2Vec_BaseOp{ ptr, count } {}

		ND_ usize	Flops () const	{ return CrossFlops(); }

		void  Run () {
			for (auto& e : data) {
				Cross_Neon_v1( OUT e.dst, e.lhs, e.rhs );
				if constexpr( c_Repeat > 1 ) {
					Cross_Neon_v1( OUT e.rhs, e.dst, e.lhs );
				}
				if constexpr( c_Repeat > 2 ) {
					Cross_Neon_v1( OUT e.lhs, e.rhs, e.dst );
					Cross_Neon_v1( OUT e.dst, e.rhs, e.lhs );
				}
				if constexpr( c_Repeat > 4 ) {
					Cross_Neon_v1( OUT e.rhs, e.dst, e.lhs );
					Cross_Neon_v1( OUT e.dst, e.lhs, e.rhs );
				}
			}
		}
	};
#endif

	template <typename VecT>
	static void  Cross_v1  (OUT VecT &dstX, OUT VecT &dstY, OUT VecT &dstZ,
							const VecT &lhsX, const VecT &lhsY, const VecT &lhsZ,
							const VecT &rhsX, const VecT &rhsY, const VecT &rhsZ)
	{
		for (usize i = 0; i < dstX.Count(); ++i)
		{
			dstX[i] = lhsY[i] * rhsZ[i] - lhsZ[i] * rhsY[i];
			dstY[i] = lhsZ[i] * rhsX[i] - lhsX[i] * rhsZ[i];
			dstZ[i] = lhsX[i] * rhsY[i] - lhsY[i] * rhsX[i];
		}
	}

	template <typename VecT>
	static void  Cross_v2  (OUT VecT &dstX, OUT VecT &dstY, OUT VecT &dstZ,
							const VecT &lhsX, const VecT &lhsY, const VecT &lhsZ,
							const VecT &rhsX, const VecT &rhsY, const VecT &rhsZ)
	{
		for (usize i = 0; i < dstX.Count(); ++i)
			dstX[i] = lhsY[i] * rhsZ[i] - lhsZ[i] * rhsY[i];

		for (usize i = 0; i < dstY.Count(); ++i)
			dstY[i] = lhsZ[i] * rhsX[i] - lhsX[i] * rhsZ[i];

		for (usize i = 0; i < dstZ.Count(); ++i)
			dstZ[i] = lhsX[i] * rhsY[i] - lhsY[i] * rhsX[i];
	}

	template <typename VecT>
	static void  Cross_v3  (OUT VecT &dstX, OUT VecT &dstY, OUT VecT &dstZ,
							const VecT &lhsX, const VecT &lhsY, const VecT &lhsZ,
							const VecT &rhsX, const VecT &rhsY, const VecT &rhsZ)
	{
		for (usize i = 0; i < dstX.Count(); ++i)
		{
			auto	tx = lhsZ[i] * rhsY[i];
			auto	ty = lhsX[i] * rhsZ[i];
			auto	tz = lhsY[i] * rhsX[i];

			dstX[i] = lhsY[i] * rhsZ[i] - tx;
			dstY[i] = lhsZ[i] * rhsX[i] - ty;
			dstZ[i] = lhsX[i] * rhsY[i] - tz;
		}
	}


	struct CrossV1_Op : A3Vec_BaseOp
	{
		CrossV1_Op (void* ptr, usize count) : A3Vec_BaseOp{ ptr, count } {}

		ND_ usize	Flops () const	{ return CrossFlops(); }

		void  Run () {
			for (auto& e : data) {
				Cross_v1( OUT e.dstX, OUT e.dstY, OUT e.dstZ,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
				if constexpr( c_Repeat > 1 ) {
					Cross_v1( OUT e.lhsX, OUT e.lhsY, OUT e.lhsZ,  e.dstX, e.rhsX, e.rhsZ,  e.dstY, e.dstZ, e.rhsY );
				}
				if constexpr( c_Repeat > 2 ) {
					Cross_v1( OUT e.dstX, OUT e.dstY, OUT e.dstZ,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
					Cross_v1( OUT e.lhsX, OUT e.lhsY, OUT e.lhsZ,  e.dstX, e.rhsX, e.rhsZ,  e.dstY, e.dstZ, e.rhsY );
				}
				if constexpr( c_Repeat > 4 ) {
					Cross_v1( OUT e.lhsX, OUT e.lhsY, OUT e.lhsZ,  e.dstX, e.rhsX, e.rhsZ,  e.dstY, e.dstZ, e.rhsY );
					Cross_v1( OUT e.dstX, OUT e.dstY, OUT e.dstZ,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
				}
			}
		}
	};

	struct CrossV2_Op : A3Vec_BaseOp
	{
		CrossV2_Op (void* ptr, usize count) : A3Vec_BaseOp{ ptr, count } {}

		ND_ usize	Flops () const	{ return CrossFlops(); }

		void  Run () {
			for (auto& e : data) {
				Cross_v2( OUT e.dstX, OUT e.dstY, OUT e.dstZ,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
				if constexpr( c_Repeat > 1 ) {
					Cross_v2( OUT e.lhsX, OUT e.lhsY, OUT e.lhsZ,  e.dstX, e.rhsX, e.rhsZ,  e.dstY, e.dstZ, e.rhsY );
				}
				if constexpr( c_Repeat > 2 ) {
					Cross_v2( OUT e.dstX, OUT e.dstY, OUT e.dstZ,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
					Cross_v2( OUT e.lhsX, OUT e.lhsY, OUT e.lhsZ,  e.dstX, e.rhsX, e.rhsZ,  e.dstY, e.dstZ, e.rhsY );
				}
				if constexpr( c_Repeat > 4 ) {
					Cross_v2( OUT e.lhsX, OUT e.lhsY, OUT e.lhsZ,  e.dstX, e.rhsX, e.rhsZ,  e.dstY, e.dstZ, e.rhsY );
					Cross_v2( OUT e.dstX, OUT e.dstY, OUT e.dstZ,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
				}
			}
		}
	};

	struct CrossV3_Op : A3Vec_BaseOp
	{
		CrossV3_Op (void* ptr, usize count) : A3Vec_BaseOp{ ptr, count } {}

		ND_ usize	Flops () const	{ return CrossFlops(); }

		void  Run () {
			for (auto& e : data) {
				Cross_v3( OUT e.dstX, OUT e.dstY, OUT e.dstZ,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
				if constexpr( c_Repeat > 1 ) {
					Cross_v3( OUT e.lhsX, OUT e.lhsY, OUT e.lhsZ,  e.dstX, e.rhsX, e.rhsZ,  e.dstY, e.dstZ, e.rhsY );
				}
				if constexpr( c_Repeat > 2 ) {
					Cross_v3( OUT e.dstX, OUT e.dstY, OUT e.dstZ,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
					Cross_v3( OUT e.lhsX, OUT e.lhsY, OUT e.lhsZ,  e.dstX, e.rhsX, e.rhsZ,  e.dstY, e.dstZ, e.rhsY );
				}
				if constexpr( c_Repeat > 4 ) {
					Cross_v3( OUT e.lhsX, OUT e.lhsY, OUT e.lhsZ,  e.dstX, e.rhsX, e.rhsZ,  e.dstY, e.dstZ, e.rhsY );
					Cross_v3( OUT e.dstX, OUT e.dstY, OUT e.dstZ,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
				}
			}
		}
	};


#ifdef AE_SIMD_SimdFloat8
	struct CrossW1_Op : D3Vec_BaseOp
	{
		CrossW1_Op (void* ptr, usize count) : D3Vec_BaseOp{ ptr, count } {}

		ND_ usize	Flops () const	{ return CrossFlops(); }

		void  Run () {
			for (auto& e : data) {
				Cross_v1( OUT e.dstX, OUT e.dstY, OUT e.dstZ,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
				if constexpr( c_Repeat > 1 ) {
					Cross_v1( OUT e.lhsX, OUT e.lhsY, OUT e.lhsZ,  e.dstX, e.rhsX, e.rhsZ,  e.dstY, e.dstZ, e.rhsY );
				}
				if constexpr( c_Repeat > 2 ) {
					Cross_v1( OUT e.dstX, OUT e.dstY, OUT e.dstZ,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
					Cross_v1( OUT e.lhsX, OUT e.lhsY, OUT e.lhsZ,  e.dstX, e.rhsX, e.rhsZ,  e.dstY, e.dstZ, e.rhsY );
				}
				if constexpr( c_Repeat > 4 ) {
					Cross_v1( OUT e.lhsX, OUT e.lhsY, OUT e.lhsZ,  e.dstX, e.rhsX, e.rhsZ,  e.dstY, e.dstZ, e.rhsY );
					Cross_v1( OUT e.dstX, OUT e.dstY, OUT e.dstZ,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
				}
			}
		}
	};

	struct CrossW2_Op : D3Vec_BaseOp
	{
		CrossW2_Op (void* ptr, usize count) : D3Vec_BaseOp{ ptr, count } {}

		ND_ usize	Flops () const	{ return CrossFlops(); }

		void  Run () {
			for (auto& e : data) {
				Cross_v2( OUT e.dstX, OUT e.dstY, OUT e.dstZ,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
				if constexpr( c_Repeat > 1 ) {
					Cross_v2( OUT e.lhsX, OUT e.lhsY, OUT e.lhsZ,  e.dstX, e.rhsX, e.rhsZ,  e.dstY, e.dstZ, e.rhsY );
				}
				if constexpr( c_Repeat > 2 ) {
					Cross_v2( OUT e.dstX, OUT e.dstY, OUT e.dstZ,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
					Cross_v2( OUT e.lhsX, OUT e.lhsY, OUT e.lhsZ,  e.dstX, e.rhsX, e.rhsZ,  e.dstY, e.dstZ, e.rhsY );
				}
				if constexpr( c_Repeat > 4 ) {
					Cross_v2( OUT e.lhsX, OUT e.lhsY, OUT e.lhsZ,  e.dstX, e.rhsX, e.rhsZ,  e.dstY, e.dstZ, e.rhsY );
					Cross_v2( OUT e.dstX, OUT e.dstY, OUT e.dstZ,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
				}
			}
		}
	};

	struct CrossW3_Op : D3Vec_BaseOp
	{
		CrossW3_Op (void* ptr, usize count) : D3Vec_BaseOp{ ptr, count } {}

		ND_ usize	Flops () const	{ return CrossFlops(); }

		void  Run () {
			for (auto& e : data) {
				Cross_v3( OUT e.dstX, OUT e.dstY, OUT e.dstZ,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
				if constexpr( c_Repeat > 1 ) {
					Cross_v3( OUT e.lhsX, OUT e.lhsY, OUT e.lhsZ,  e.dstX, e.rhsX, e.rhsZ,  e.dstY, e.dstZ, e.rhsY );
				}
				if constexpr( c_Repeat > 2 ) {
					Cross_v3( OUT e.dstX, OUT e.dstY, OUT e.dstZ,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
					Cross_v3( OUT e.lhsX, OUT e.lhsY, OUT e.lhsZ,  e.dstX, e.rhsX, e.rhsZ,  e.dstY, e.dstZ, e.rhsY );
				}
				if constexpr( c_Repeat > 4 ) {
					Cross_v3( OUT e.lhsX, OUT e.lhsY, OUT e.lhsZ,  e.dstX, e.rhsX, e.rhsZ,  e.dstY, e.dstZ, e.rhsY );
					Cross_v3( OUT e.dstX, OUT e.dstY, OUT e.dstZ,  e.lhsX, e.lhsY, e.lhsZ,  e.rhsX, e.rhsY, e.rhsZ );
				}
			}
		}
	};
#endif


	template <typename Op>
	static void  RunOp (IntervalProfiler &profiler, StringView name, void* ptr, usize count) __NE___
	{
		Op		op{ ptr, count };
		HashVal	h;

		AE_LOGI( name );
		profiler.BeginTest( name,
							[s=op.Size(), f=op.Flops(), c=op.VecCount()] (secondsd dt)
							{
								return	ToStringSfx(double(c) / dt.count()) << "Op/s|" <<		// processed vectors
										ToStringSfx(double(f) / dt.count()) << "FLOPS|" <<
										ToStringSfx(double(usize(s)) / dt.count()) << "B/s";
							});

		for (uint i = 0; i < c_MaxIter; ++i)
		{
			op.Init();
			MemoryBarrier( EMemoryOrder::AcquireRelease );

			profiler.BeginIteration();

			op.Run();

			profiler.EndIteration();

			h += op.Hash();
		}
		profiler.EndTest();
	}


	static void  DotProductPerf (const String &name)
	{
		IntervalProfiler	profiler{ "Vertical SIMD, Dot test, "s << name,
									  IntervalProfiler::EFlags::SortByPerf | IntervalProfiler::EFlags::ExcludePerfDiff };

		DynUntypedStorage	st0;
		TEST( st0.Alloc( 512_MiB, 4_KiB, null ));

		const usize		count = Min( usize(st0.Size() / SizeOf<A2Vec>),
									 usize(st0.Size() / SizeOf<B2Vec>),
									#ifdef AE_SIMD_SimdFloat8
									 (usize(st0.Size() / SizeOf<D2Vec>)*sizeof(DVec))/sizeof(AVec)
									#else
									 ~usize(0)
									#endif
									);

		AE_LOGI( "Array size: "s << ToString(count) );
		AE_LOGI( "SSE vec count: "s << ToString(B2Vec_BaseOp{ st0.Data(), count }.VecCount()) );
		AE_LOGI( "Vert SSE vec count: "s << ToString(A2Vec_BaseOp{ st0.Data(), count }.VecCount()) );
	  #ifdef AE_SIMD_SimdFloat8
		AE_LOGI( "Vert AVX vec count: "s << ToString(D2Vec_BaseOp{ st0.Data(), count }.VecCount()) );
	  #endif

	  #if AE_SIMD_SSE >= 20
		RunOp< Dot3_SSE_Op	>( profiler, "Dot3 SSE2",		st0.Data(), count );
	  #endif
	  #if AE_SIMD_SSE >= 30
		RunOp< Dot3_SSE30_Op>( profiler, "Dot3 SSE3",		st0.Data(), count );
	  #endif
	  #if AE_SIMD_SSE >= 41
		RunOp< Dot3_SSE41_Op>( profiler, "Dot3 SSE4.1",		st0.Data(), count );
	  #endif
	  #if AE_SIMD_NEON
		RunOp< Dot3_Neon1_Op>( profiler, "Dot3 Neon v1",	st0.Data(), count );
		RunOp< Dot3_Neon2_Op>( profiler, "Dot3 Neon v2",	st0.Data(), count );
	  #endif
	  #if AE_SIMD_FMA
		RunOp< DotV1_Op		>( profiler, "vert Dot v1",		st0.Data(), count );
		RunOp< DotV5_Op		>( profiler, "vert Dot v5",		st0.Data(), count );
	  #endif
		RunOp< DotV2_Op		>( profiler, "vert Dot v2",		st0.Data(), count );
		RunOp< DotV3_Op		>( profiler, "vert Dot v3",		st0.Data(), count );
		RunOp< DotV4_Op		>( profiler, "vert Dot v4",		st0.Data(), count );
	  #ifdef AE_SIMD_SimdFloat8
	  # if AE_SIMD_FMA
		RunOp< DotW1_Op		>( profiler, "vert DotAVX v1",	st0.Data(), count );
		RunOp< DotW5_Op		>( profiler, "vert DotAVX v5",	st0.Data(), count );
	  # endif
		RunOp< DotW2_Op		>( profiler, "vert DotAVX v2",	st0.Data(), count );
		RunOp< DotW3_Op		>( profiler, "vert DotAVX v3",	st0.Data(), count );
		RunOp< DotW4_Op		>( profiler, "vert DotAVX v4",	st0.Data(), count );
	  #endif
	}


	static void  CrossProductPerf (const String &name)
	{
		IntervalProfiler	profiler{ "Vertical SIMD, Cross test, "s << name,
									  IntervalProfiler::EFlags::SortByPerf | IntervalProfiler::EFlags::ExcludePerfDiff };

		DynUntypedStorage	st0;
		TEST( st0.Alloc( 512_MiB, 4_KiB, null ));

		const usize		count = Min( usize(st0.Size() / SizeOf<A3Vec>),
									 usize(st0.Size() / SizeOf<B2Vec>),
									#ifdef AE_SIMD_SimdFloat8
									 (usize(st0.Size() / SizeOf<D3Vec>)*sizeof(DVec))/sizeof(AVec)
									#else
									 ~usize(0)
									#endif
									);

		AE_LOGI( "Array size: "s << ToString(count) );
		AE_LOGI( "SSE vec count: "s << ToString(B2Vec_BaseOp{ st0.Data(), count }.VecCount()) );
		AE_LOGI( "Vert SSE vec count: "s << ToString(A3Vec_BaseOp{ st0.Data(), count }.VecCount()) );
	  #ifdef AE_SIMD_SimdFloat8
		AE_LOGI( "Vert AVX vec count: "s << ToString(D3Vec_BaseOp{ st0.Data(), count }.VecCount()) );
	  #endif

	  #if AE_SIMD_SSE >= 20
		RunOp< Cross_SSEv1_Op	>( profiler, "Cross SSE2 v1",		st0.Data(), count );
		RunOp< Cross_SSEv2_Op	>( profiler, "Cross SSE2 v2",		st0.Data(), count );
	  #endif
	  #if AE_SIMD_NEON
		RunOp< Cross_Neon1_Op	>( profiler, "Cross Neon v1",		st0.Data(), count );
	  #endif
		RunOp< CrossV1_Op		>( profiler, "vert Cross v1",		st0.Data(), count );
		RunOp< CrossV2_Op		>( profiler, "vert Cross v2",		st0.Data(), count );
		RunOp< CrossV3_Op		>( profiler, "vert Cross v3",		st0.Data(), count );
	  #ifdef AE_SIMD_SimdFloat8
		RunOp< CrossW1_Op		>( profiler, "vert CrossAVX v1",	st0.Data(), count );
		RunOp< CrossW2_Op		>( profiler, "vert CrossAVX v2",	st0.Data(), count );
		RunOp< CrossW3_Op		>( profiler, "vert CrossAVX v3",	st0.Data(), count );
	  #endif
	}

} // namespace


extern void PerfTest_VertSIMD ()
{
	ForEachCoreType(
		[] (auto& core, Function<void()> setAffinity)
		{
			setAffinity();

			const String	name = String{ToString( core.type )} << " core";

			DotProductPerf( name );
			CrossProductPerf( name );
		});

	TEST_PASSED();
}

#else

extern void PerfTest_VertSIMD ()
{}

#endif // AE_SIMD_SimdFloat4
