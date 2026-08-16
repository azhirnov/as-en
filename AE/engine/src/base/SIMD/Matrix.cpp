// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "base/SIMD/Matrix.h"

#ifdef AE_COMPILER_MSVC
#	define RESTRICT __restrict
#else
#	define RESTRICT __restrict__
#endif

namespace AE::Base
{
#if AE_SIMD_AVX >= 1
# if AE_SIMD_FMA
#	define FMA(_a_, _b_, _c_)	_mm256_fmadd_ps( (_a_), (_b_), (_c_) )
# else
#	define FMA(_a_, _b_, _c_)	_mm256_add_ps( _mm256_mul_ps( (_a_), (_b_) ), (_c_) );
# endif
#endif


/*
=================================================
	MatrixMulAdd
----
	60% of theoretical performance on large matrix (1024x1024),
	80% of theoretical performance on small matrix (256x256 - 512x512),
	but with small and readable code
=================================================
*/
#if AE_SIMD_AVX >= 1
namespace GEMM_AVX
{
	static void  Microkernel_6x16 (const float*	RESTRICT	A, const uint lda,
								   const float*	RESTRICT	B, const uint ldb,
								   INOUT float* RESTRICT	C, const uint ldc,
								   const uint				K) __NE___
	{
		__m256	c00 = _mm256_loadu_ps( C + 0 * ldc + 0 );
		__m256	c01 = _mm256_loadu_ps( C + 0 * ldc + 8 );
		__m256	c10 = _mm256_loadu_ps( C + 1 * ldc + 0 );
		__m256	c11 = _mm256_loadu_ps( C + 1 * ldc + 8 );
		__m256	c20 = _mm256_loadu_ps( C + 2 * ldc + 0 );
		__m256	c21 = _mm256_loadu_ps( C + 2 * ldc + 8 );
		__m256	c30 = _mm256_loadu_ps( C + 3 * ldc + 0 );
		__m256	c31 = _mm256_loadu_ps( C + 3 * ldc + 8 );
		__m256	c40 = _mm256_loadu_ps( C + 4 * ldc + 0 );
		__m256	c41 = _mm256_loadu_ps( C + 4 * ldc + 8 );
		__m256	c50 = _mm256_loadu_ps( C + 5 * ldc + 0 );
		__m256	c51 = _mm256_loadu_ps( C + 5 * ldc + 8 );

		for (uint k = 0; k < K; ++k)
		{
			__m256	b0  = _mm256_loadu_ps( B + k * ldb + 0 );
			__m256	b1  = _mm256_loadu_ps( B + k * ldb + 8 );

			__m256	a0  = _mm256_set1_ps( A[0 * lda + k] );
					c00 = FMA( a0, b0, c00 );
					c01 = FMA( a0, b1, c01 );

			__m256	a1  = _mm256_set1_ps( A[1 * lda + k] );
					c10 = FMA( a1, b0, c10 );
					c11 = FMA( a1, b1, c11 );

			__m256	a2  = _mm256_set1_ps( A[2 * lda + k] );
					c20 = FMA( a2, b0, c20 );
					c21 = FMA( a2, b1, c21 );

			__m256	a3  = _mm256_set1_ps( A[3 * lda + k] );
					c30 = FMA( a3, b0, c30 );
					c31 = FMA( a3, b1, c31 );

			__m256	a4  = _mm256_set1_ps( A[4 * lda + k] );
					c40 = FMA( a4, b0, c40 );
					c41 = FMA( a4, b1, c41 );

			__m256	a5  = _mm256_set1_ps( A[5 * lda + k] );
					c50 = FMA( a5, b0, c50 );
					c51 = FMA( a5, b1, c51 );
		}

		_mm256_storeu_ps( OUT C + 0 * ldc + 0, c00 );
		_mm256_storeu_ps( OUT C + 0 * ldc + 8, c01 );
		_mm256_storeu_ps( OUT C + 1 * ldc + 0, c10 );
		_mm256_storeu_ps( OUT C + 1 * ldc + 8, c11 );
		_mm256_storeu_ps( OUT C + 2 * ldc + 0, c20 );
		_mm256_storeu_ps( OUT C + 2 * ldc + 8, c21 );
		_mm256_storeu_ps( OUT C + 3 * ldc + 0, c30 );
		_mm256_storeu_ps( OUT C + 3 * ldc + 8, c31 );
		_mm256_storeu_ps( OUT C + 4 * ldc + 0, c40 );
		_mm256_storeu_ps( OUT C + 4 * ldc + 8, c41 );
		_mm256_storeu_ps( OUT C + 5 * ldc + 0, c50 );
		_mm256_storeu_ps( OUT C + 5 * ldc + 8, c51 );
	}

	static void  Microkernel_1x16 (const float*	RESTRICT	A,
								   const float*	RESTRICT	B,
								   INOUT float*	RESTRICT	C,
								   const uint				ldb,
								   const uint				K) __NE___
	{
		__m256	c0 = _mm256_loadu_ps( C);
		__m256	c1 = _mm256_loadu_ps( C + 8 );

		for (uint k = 0; k < K; ++k)
		{
			__m256	a  = _mm256_set1_ps( A[k] );
			__m256	b0 = _mm256_loadu_ps( B + k * ldb );
			__m256	b1 = _mm256_loadu_ps( B + k * ldb + 8 );

					c0 = FMA(a, b0, c0 );
					c1 = FMA(a, b1, c1 );
		}

		_mm256_storeu_ps( OUT C,     c0 );
		_mm256_storeu_ps( OUT C + 8, c1 );
	}

	static __m256i  Mask8 (uint count) __NE___
	{
		return _mm256_setr_epi32(
			count > 0 ? -1 : 0,
			count > 1 ? -1 : 0,
			count > 2 ? -1 : 0,
			count > 3 ? -1 : 0,
			count > 4 ? -1 : 0,
			count > 5 ? -1 : 0,
			count > 6 ? -1 : 0,
			count > 7 ? -1 : 0);
	}

	static void  Microkernel_1x16_Masked (const float* RESTRICT	A,
										  const float* RESTRICT	B,
										  INOUT float* RESTRICT	C,
										  const uint			ldb,
										  const uint			K,
										  const uint			count) __NE___
	{
		const uint		count0	= Min( count, 8u );
		const uint		count1	= count > 8 ? count - 8 : 0;

		const __m256i	mask0	= Mask8( count0 );
		__m256			c0		= _mm256_maskload_ps( C, mask0 );

		// avoid even constructing C + 8 / B + 8 when there is no second part.
		if ( count1 == 0 )
		{
			for (uint k = 0; k < K; ++k)
			{
				__m256	a  = _mm256_set1_ps( A[k] );
				__m256	b0 = _mm256_maskload_ps( B + k * ldb, mask0 );
						c0 = FMA( a, b0, c0 );
			}

			_mm256_maskstore_ps( OUT C, mask0, c0 );
			return;
		}

		const __m256i	mask1	= Mask8( count1 );
		__m256			c1		= _mm256_maskload_ps( C + 8, mask1 );

		for (uint k = 0; k < K; ++k)
		{
			__m256		 a  = _mm256_set1_ps(A[k]);
			const float* bk = B + k * ldb;

			__m256	b0 = _mm256_loadu_ps( bk ); // count > 8, so all 8 are valid
			__m256	b1 = _mm256_maskload_ps( bk + 8, mask1 );
					c0 = FMA( a, b0, c0 );
					c1 = FMA( a, b1, c1 );
		}

		_mm256_storeu_ps( OUT C, c0 );
		_mm256_maskstore_ps( OUT C + 8, mask1, c1 );
	}

	static void  MatrixMulAdd (const float*	RESTRICT	A,
							   const float*	RESTRICT	B,
							   INOUT float*	RESTRICT	C,
							   const uint				M,
							   const uint				N,
							   const uint				K) __NE___
	{
		const uint	lda = K;
		const uint	ldb = N;
		const uint	ldc = N;

		const uint	M6  = M - (M % 6);
		const uint	N16 = N - (N % 16);

		for (uint i = 0; i < M6; i += 6)
		{
			const float*	A_tile	= A + (i * lda);

			// complete 6x16 tiles
			for (uint j = 0; j < N16; j += 16)
			{
				const float*	B_tile	= B + j;
				float*			C_tile	= C + (i * ldc) + j;

				Microkernel_6x16( A_tile, lda, B_tile, ldb, INOUT C_tile, ldc, K );
			}

			// tail
			if_unlikely( N16 != N )
			{
				for (uint j = 0; j < 6; ++j)
				{
					float* RESTRICT	C_tile	= C + (i * ldc);

					Microkernel_1x16_Masked(
						A_tile + j * lda,
						B + N16,
						INOUT C_tile + j * ldc + N16,
						ldb,
						K,
						N - N16
					);
				}
			}
		}

		// tail
		for (uint i = M6; i < M; ++i)
		{
			const float*	A_row = A + i * lda;
			float*      	C_row = C + i * ldc;

			for (uint j = 0; j < N16; j += 16)
			{
				Microkernel_1x16(
					A_row,
					B + j,
					INOUT C_row + j,
					ldb,
					K);
			}

			if_unlikely( N16 != N )
			{
				Microkernel_1x16_Masked(
					A_row,
					B + N16,
					INOUT C_row + N16,
					ldb,
					K,
					N - N16
				);
			}
		}
	}

} // GEMM_AVX
#endif // AVX1

/*
=================================================
	MatVecMulAdd
----
	40% of theoretical performance on large vector/matrix (1024x1024)
=================================================
*/
#if AE_SIMD_AVX >= 1
namespace GEMV_AVX
{
	inline __m128  HSum4x8 (const __m256 a0, const __m256 a1,
							const __m256 a2, const __m256 a3) __NE___
	{
		// -> [ hsum(a0), hsum(a1), hsum(a2), hsum(a3) ]
		__m256  t0 = _mm256_hadd_ps( a0, a1 );
		__m256  t1 = _mm256_hadd_ps( a2, a3 );
		__m256  t2 = _mm256_hadd_ps( t0, t1 );
		return _mm_add_ps( _mm256_castps256_ps128( t2 ), _mm256_extractf128_ps( t2, 1 ));
	}

	static void  MatVecMulAdd (const float* RESTRICT A,   // A[K]
							   const float* RESTRICT B,   // B[M][K]
							   INOUT float* RESTRICT C,   // C[M]
							   const uint			 M,
							   const uint			 K) __NE___
	{
		uint m = 0;
		for (; m + 4 <= M; m += 4)
		{
			const float* RESTRICT	b0 = B + (m + 0) * K;
			const float* RESTRICT	b1 = B + (m + 1) * K;
			const float* RESTRICT	b2 = B + (m + 2) * K;
			const float* RESTRICT	b3 = B + (m + 3) * K;

			__m256 acc00 = _mm256_setzero_ps(), acc01 = _mm256_setzero_ps();
			__m256 acc10 = _mm256_setzero_ps(), acc11 = _mm256_setzero_ps();
			__m256 acc20 = _mm256_setzero_ps(), acc21 = _mm256_setzero_ps();
			__m256 acc30 = _mm256_setzero_ps(), acc31 = _mm256_setzero_ps();

			uint k = 0;
			for (; k + 16 <= K; k += 16)
			{
				__m256 a0 = _mm256_loadu_ps( A + k + 0 );
				__m256 a1 = _mm256_loadu_ps( A + k + 8 );

				acc00 = FMA( a0, _mm256_loadu_ps( b0 + k + 0 ), acc00 );
				acc01 = FMA( a1, _mm256_loadu_ps( b0 + k + 8 ), acc01 );
				acc10 = FMA( a0, _mm256_loadu_ps( b1 + k + 0 ), acc10 );
				acc11 = FMA( a1, _mm256_loadu_ps( b1 + k + 8 ), acc11 );
				acc20 = FMA( a0, _mm256_loadu_ps( b2 + k + 0 ), acc20 );
				acc21 = FMA( a1, _mm256_loadu_ps( b2 + k + 8 ), acc21 );
				acc30 = FMA( a0, _mm256_loadu_ps( b3 + k + 0 ), acc30 );
				acc31 = FMA( a1, _mm256_loadu_ps( b3 + k + 8 ), acc31 );
			}

			if ( k + 8 <= K )
			{
				__m256 a0 = _mm256_loadu_ps( A + k );
				acc00 = FMA( a0, _mm256_loadu_ps( b0 + k ), acc00 );
				acc10 = FMA( a0, _mm256_loadu_ps( b1 + k ), acc10 );
				acc20 = FMA( a0, _mm256_loadu_ps( b2 + k ), acc20 );
				acc30 = FMA( a0, _mm256_loadu_ps( b3 + k ), acc30 );
				k += 8;
			}

			acc00 = _mm256_add_ps( acc00, acc01 );
			acc10 = _mm256_add_ps( acc10, acc11 );
			acc20 = _mm256_add_ps( acc20, acc21 );
			acc30 = _mm256_add_ps( acc30, acc31 );

			float t[4];
			_mm_storeu_ps( t, HSum4x8( acc00, acc10, acc20, acc30 ));

			// scalar tail
			for (; k < K; ++k)
			{
				float a = A[k];
				t[0] += b0[k] * a;
				t[1] += b1[k] * a;
				t[2] += b2[k] * a;
				t[3] += b3[k] * a;
			}

			C[m+0] += t[0];
			C[m+1] += t[1];
			C[m+2] += t[2];
			C[m+3] += t[3];
		}

		// row tail: single-row dot product, 4 accumulators
		for (; m < M; ++m)
		{
			const float* RESTRICT b = B + m * K;
			__m256 acc0 = _mm256_setzero_ps(), acc1 = _mm256_setzero_ps();
			__m256 acc2 = _mm256_setzero_ps(), acc3 = _mm256_setzero_ps();

			uint k = 0;
			for (; k + 32 <= K; k += 32)
			{
				acc0 = FMA( _mm256_loadu_ps( A+k+ 0 ), _mm256_loadu_ps( b+k+ 0 ), acc0 );
				acc1 = FMA( _mm256_loadu_ps( A+k+ 8 ), _mm256_loadu_ps( b+k+ 8 ), acc1 );
				acc2 = FMA( _mm256_loadu_ps( A+k+16 ), _mm256_loadu_ps( b+k+16 ), acc2 );
				acc3 = FMA( _mm256_loadu_ps( A+k+24 ), _mm256_loadu_ps( b+k+24 ), acc3 );
			}

			for (; k + 8 <= K; k += 8)
			{
				acc0 = FMA( _mm256_loadu_ps( A+k ), _mm256_loadu_ps( b+k ), acc0 );
			}

			acc0 = _mm256_add_ps( _mm256_add_ps( acc0, acc1 ), _mm256_add_ps( acc2, acc3 ));

			__m128 s = _mm_add_ps( _mm256_castps256_ps128( acc0 ), _mm256_extractf128_ps( acc0, 1 ));

			s = _mm_add_ps( s, _mm_movehl_ps( s, s ));
			s = _mm_add_ss( s, _mm_movehdup_ps( s ));

			float sum = _mm_cvtss_f32( s );

			for (; k < K; ++k)
			{
				sum += b[k] * A[k];
			}
			C[m] += sum;
		}
	}

} // GEMV_AVX
#endif // AVX1
//-----------------------------------------------------------------------------
#undef FMA


/*
=================================================
	MatrixMulAdd
----
	40% of theoretical performance on large matrix (1024x1024),
	90% of theoretical performance on small matrix (128x128)
=================================================
*/
#if AE_SIMD_NEON
namespace GEMM_Neon
{
	//---------------------------------------------------------------------
	//  Blocking (AArch64 only)
	//---------------------------------------------------------------------
	static constexpr uint GM_MR  = 8;       // micro-kernel rows
	static constexpr uint GM_NR  = 8;       // micro-kernel cols
	static constexpr uint GM_KC  = 128;     // K-block:  micro-panel = KC*8*4 =   4 KB (L1)
	static constexpr uint GM_NC  = 256;     // B block:  packed B    = KC*NC*4 = 128 KB (L2)

	//---------------------------------------------------------------------
	//  8x8 micro-kernel:  C[0:8, 0:8] += Apanel * Bpanel
	//    pa: k-major, pa[k*8 + r] = A[r][k]
	//    pb: k-major, pb[k*8 + c] = B[k][c]
	//  16 accumulators + 4 source regs = 20 of 32 q-regs -> no spills.
	//---------------------------------------------------------------------
	inline void  MicroKernel (const float* RESTRICT pa,
							  const float* RESTRICT pb,
							  float* RESTRICT pc,
							  const uint kc, const uint ldc)
	{
		float32x4_t		c00 = vld1q_f32( pc + 0*ldc + 0 );
		float32x4_t		c01 = vld1q_f32( pc + 0*ldc + 4 );
		float32x4_t		c10 = vld1q_f32( pc + 1*ldc + 0 );
		float32x4_t		c11 = vld1q_f32( pc + 1*ldc + 4 );
		float32x4_t		c20 = vld1q_f32( pc + 2*ldc + 0 );
		float32x4_t		c21 = vld1q_f32( pc + 2*ldc + 4 );
		float32x4_t		c30 = vld1q_f32( pc + 3*ldc + 0 );
		float32x4_t		c31 = vld1q_f32( pc + 3*ldc + 4 );
		float32x4_t		c40 = vld1q_f32( pc + 4*ldc + 0 );
		float32x4_t		c41 = vld1q_f32( pc + 4*ldc + 4 );
		float32x4_t		c50 = vld1q_f32( pc + 5*ldc + 0 );
		float32x4_t		c51 = vld1q_f32( pc + 5*ldc + 4 );
		float32x4_t		c60 = vld1q_f32( pc + 6*ldc + 0 );
		float32x4_t		c61 = vld1q_f32( pc + 6*ldc + 4 );
		float32x4_t		c70 = vld1q_f32( pc + 7*ldc + 0 );
		float32x4_t		c71 = vld1q_f32( pc + 7*ldc + 4 );

		for (uint k = 0; k < kc; ++k)
		{
			const float32x4_t	a0 = vld1q_f32( pa + 0 );   // rows 0..3 at depth k
			const float32x4_t	a1 = vld1q_f32( pa + 4 );   // rows 4..7 at depth k
			const float32x4_t	b0 = vld1q_f32( pb + 0 );   // cols 0..3 at depth k
			const float32x4_t	b1 = vld1q_f32( pb + 4 );   // cols 4..7 at depth k

			c00 = vfmaq_laneq_f32( c00, b0, a0, 0 );
			c01 = vfmaq_laneq_f32( c01, b1, a0, 0 );
			c10 = vfmaq_laneq_f32( c10, b0, a0, 1 );
			c11 = vfmaq_laneq_f32( c11, b1, a0, 1 );
			c20 = vfmaq_laneq_f32( c20, b0, a0, 2 );
			c21 = vfmaq_laneq_f32( c21, b1, a0, 2 );
			c30 = vfmaq_laneq_f32( c30, b0, a0, 3 );
			c31 = vfmaq_laneq_f32( c31, b1, a0, 3 );
			c40 = vfmaq_laneq_f32( c40, b0, a1, 0 );
			c41 = vfmaq_laneq_f32( c41, b1, a1, 0 );
			c50 = vfmaq_laneq_f32( c50, b0, a1, 1 );
			c51 = vfmaq_laneq_f32( c51, b1, a1, 1 );
			c60 = vfmaq_laneq_f32( c60, b0, a1, 2 );
			c61 = vfmaq_laneq_f32( c61, b1, a1, 2 );
			c70 = vfmaq_laneq_f32( c70, b0, a1, 3 );
			c71 = vfmaq_laneq_f32( c71, b1, a1, 3 );

			pa += 8;
			pb += 8;
		}

		vst1q_f32( pc + 0*ldc + 0, c00 );
		vst1q_f32( pc + 0*ldc + 4, c01 );
		vst1q_f32( pc + 1*ldc + 0, c10 );
		vst1q_f32( pc + 1*ldc + 4, c11 );
		vst1q_f32( pc + 2*ldc + 0, c20 );
		vst1q_f32( pc + 2*ldc + 4, c21 );
		vst1q_f32( pc + 3*ldc + 0, c30 );
		vst1q_f32( pc + 3*ldc + 4, c31 );
		vst1q_f32( pc + 4*ldc + 0, c40 );
		vst1q_f32( pc + 4*ldc + 4, c41 );
		vst1q_f32( pc + 5*ldc + 0, c50 );
		vst1q_f32( pc + 5*ldc + 4, c51 );
		vst1q_f32( pc + 6*ldc + 0, c60 );
		vst1q_f32( pc + 6*ldc + 4, c61 );
		vst1q_f32( pc + 7*ldc + 0, c70 );
		vst1q_f32( pc + 7*ldc + 4, c71 );
	}

	//---------------------------------------------------------------------
	inline void  Transpose4x4 (const float* s0, const float* s1,
							   const float* s2, const float* s3,
							   float* d, const uint dstride)
	{
		const float32x4_t	r0	= vld1q_f32( s0 ), r1 = vld1q_f32( s1 );
		const float32x4_t	r2	= vld1q_f32( s2 ), r3 = vld1q_f32( s3 );
		const float32x4x2_t	t01	= vtrnq_f32( r0, r1 );
		const float32x4x2_t	t23	= vtrnq_f32( r2, r3 );

		vst1q_f32( d + 0*dstride, vcombine_f32( vget_low_f32 ( t01.val[0] ), vget_low_f32 ( t23.val[0] )));
		vst1q_f32( d + 1*dstride, vcombine_f32( vget_low_f32 ( t01.val[1] ), vget_low_f32 ( t23.val[1] )));
		vst1q_f32( d + 2*dstride, vcombine_f32( vget_high_f32( t01.val[0] ), vget_high_f32( t23.val[0] )));
		vst1q_f32( d + 3*dstride, vcombine_f32( vget_high_f32( t01.val[1] ), vget_high_f32( t23.val[1] )));
	}

	// Pack an 8 x kc panel of A (row stride lda) -> k-major.  kc % 4 == 0.
	inline void  PackAPanel (const float* RESTRICT src, float* RESTRICT dst,
							 const uint kc, const uint lda)
	{
		for (uint k = 0; k < kc; k += 4)
		{
			Transpose4x4( src + 0*lda + k, src + 1*lda + k,
						  src + 2*lda + k, src + 3*lda + k,
						  dst + k*8 + 0, 8 );
			Transpose4x4( src + 4*lda + k, src + 5*lda + k,
						  src + 6*lda + k, src + 7*lda + k,
						  dst + k*8 + 4, 8 );
		}
	}

	// Pack a kc x nc block of B (row stride ldb) -> 8-wide k-major panels.
	// nc % 8 == 0.
	inline void  PackB (const float* RESTRICT src, float* RESTRICT dst,
						const uint kc, const uint nc, const uint ldb)
	{
		for (uint j = 0; j < nc; j += GM_NR)
		{
			const float*  s = src + j;
			float*        d = dst + j*kc;

			for (uint k = 0; k < kc; ++k)
			{
				vst1q_f32( d + 0, vld1q_f32( s + 0 ));
				vst1q_f32( d + 4, vld1q_f32( s + 4 ));
				s += ldb;
				d += GM_NR;
			}
		}
	}

	//=====================================================================
	//  C[M,N] += A[M,K] * B[K,N],  row-major.
	//  Requires: M, N, K are multiples of 8;  A, B, C are 32-byte aligned.
	//=====================================================================
	static void  MatrixMulAdd (const float*	RESTRICT	A,
							   const float*	RESTRICT	B,
							   INOUT float*	RESTRICT	C,
							   const uint				M,
							   const uint				N,
							   const uint				K) __NE___
	{
		alignas(64) float	packedB[GM_KC * GM_NC];   // 128 KB, stack
		alignas(64) float	packedA[GM_KC * GM_MR];   //   4 KB, stack

		for (uint jc = 0; jc < N; jc += GM_NC)
		{
			const uint nc = Min( GM_NC, N - jc );

			for (uint k0 = 0; k0 < K; k0 += GM_KC)
			{
				const uint kc = Min( GM_KC, K - k0 );

				PackB( B + k0*N + jc, OUT packedB, kc, nc, N );			// once per K-block

				for (uint ir = 0; ir < M; ir += GM_MR)
				{
					PackAPanel( A + ir*K + k0, OUT packedA, kc, K );	// 4 KB, L1-resident

					float* RESTRICT	cPanel = C + ir*N + jc;

					for (uint jr = 0; jr < nc; jr += GM_NR)
					{
						MicroKernel( packedA, packedB + jr*kc, OUT cPanel + jr, kc, N );
					}
				}
			}
		}
	}

} // GEMM_Neon
#endif // NEON
//-----------------------------------------------------------------------------

/*
=================================================
	MatVecMulAdd
----
	% of theoretical performance on large vector/matrix (1024x1024)
=================================================
*/
#if AE_SIMD_NEON
namespace GEMV_Neon
{
	static void MatVecMulAdd(const float* RESTRICT	A,   // A[K]
							 const float* RESTRICT	B,   // B[M][K]
							 INOUT float* RESTRICT	C,   // C[M]
							 const uint				M,
							 const uint				K) __NE___
	{
		uint	i = 0;
		for (; i + 3 < M; i += 4)
		{
			const float* rowB0 = B + (i + 0) * K;
			const float* rowB1 = B + (i + 1) * K;
			const float* rowB2 = B + (i + 2) * K;
			const float* rowB3 = B + (i + 3) * K;

			float32x4_t acc0 = vdupq_n_f32( C[i + 0] );
			float32x4_t acc1 = vdupq_n_f32( C[i + 1] );
			float32x4_t acc2 = vdupq_n_f32( C[i + 2] );
			float32x4_t acc3 = vdupq_n_f32( C[i + 3] );

			uint	j = 0;
			for (; j + 3 < K; j += 4)
			{
				float32x4_t	vecA = vld1q_f32( A + j );

				float32x4_t	vecB0 = vld1q_f32( rowB0 + j );
				float32x4_t	vecB1 = vld1q_f32( rowB1 + j );
				float32x4_t	vecB2 = vld1q_f32( rowB2 + j );
				float32x4_t	vecB3 = vld1q_f32( rowB3 + j );

				acc0 = vfmaq_f32( acc0, vecA, vecB0 );
				acc1 = vfmaq_f32( acc1, vecA, vecB1 );
				acc2 = vfmaq_f32( acc2, vecA, vecB2 );
				acc3 = vfmaq_f32( acc3, vecA, vecB3 );
			}

			C[i + 0] = vaddvq_f32( acc0 );
			C[i + 1] = vaddvq_f32( acc1 );
			C[i + 2] = vaddvq_f32( acc2 );
			C[i + 3] = vaddvq_f32( acc3 );

			for (; j < K; ++j)
			{
				float valA = A[j];
				C[i + 0] += valA * rowB0[j];
				C[i + 1] += valA * rowB1[j];
				C[i + 2] += valA * rowB2[j];
				C[i + 3] += valA * rowB3[j];
			}
		}

		// tail
		for (; i < M; ++i)
		{
			const float*	rowB	= B + i * K;
			float32x4_t		acc		= vdupq_n_f32(0.0f);

			uint	j = 0;
			for (; j + 3 < K; j += 4)
			{
				float32x4_t	vecA	= vld1q_f32( A + j );
				float32x4_t	vecB	= vld1q_f32( rowB + j );
							acc		= vfmaq_f32( acc, vecA, vecB );
			}

			C[i] += vaddvq_f32( acc );

			for (; j < K; ++j) {
				C[i] += A[j] * rowB[j];
			}
		}
	}

} // GEMV_AVX
#endif // NEON
//-----------------------------------------------------------------------------


#undef RESTRICT

namespace
{
#if AE_SIMD_AVX >= 30
	const Bytes		c_Align = 64_b;

#elif AE_SIMD_AVX >= 1
	const Bytes		c_Align = 32_b;

#elif AE_SIMD_SSE >= 20 or AE_SIMD_NEON
	const Bytes		c_Align = 16_b;
#else
	const Bytes		c_Align = 4_b;
#endif
}

/*
=================================================
	IsValid
=================================================
*/
	bool  GEMM::IsValid (LargeMatrixView<const float, EMatrixLayout::RowMajor> mat) __NE___
	{
		return IsMultipleOf( mat.Data().get(), c_Align );
	}

	bool  GEMV::IsValid (LargeVectorView<const float> vec) __NE___
	{
		return IsMultipleOf( vec.Data().get(), c_Align );
	}

	bool  GEMM::IsValidDim (LargeMatrixView<const float,	EMatrixLayout::RowMajor>  matA,
							LargeMatrixView<const float,	EMatrixLayout::RowMajor>  matB,
							LargeMatrixView<const float,	EMatrixLayout::RowMajor>  matC) __NE___
	{
		const uint	M = matA.Dimension().rows;
		const uint	N = matB.Dimension().columns;
		const uint	K = matA.Dimension().columns;

		CHECK_ERR( matA.Dimension()	== MatrixDim(K,M) );
		CHECK_ERR( matB.Dimension()	== MatrixDim(N,K) );
		CHECK_ERR( matC.Dimension()	== MatrixDim(N,M) );
		return true;
	}

	bool  GEMV::IsValidDim (LargeVectorView<const float>							vecA,
							LargeMatrixView<const float, EMatrixLayout::RowMajor>	matB,
							LargeVectorView<const float>							vecC) __NE___
	{
		CHECK_ERR( matB.Dimension().rows	== vecC.Length() );		// M
		CHECK_ERR( matB.Dimension().columns == vecA.Length() );		// K
		return true;
	}

/*
=================================================
	MulAdd (RowMajor)
=================================================
*/
	void  GEMM::MulAdd (LargeMatrixView<const float,	EMatrixLayout::RowMajor>  matA,			// MxK
						LargeMatrixView<const float,	EMatrixLayout::RowMajor>  matB,			// KxN
						INOUT LargeMatrixView<float,	EMatrixLayout::RowMajor>  matC) __NE___	// MxN
	{
		const uint	M = matA.Dimension().rows;
		const uint	N = matB.Dimension().columns;
		const uint	K = matA.Dimension().columns;

		ASSERT( IsValid( matA ));
		ASSERT( IsValid( matB ));
		ASSERT( IsValid( matC ));
		ASSERT( IsValidDim( matA, matB, matC ));

	#if 0 //AE_SIMD_AVX >= 30
		// not implemented

	#elif AE_SIMD_AVX >= 1
		AE_INLINE_ALL
		GEMM_AVX::MatrixMulAdd( matA.Data().get(), matB.Data().get(), matC.Data().get(), M, N, K );

	#elif AE_SIMD_SSE >= 20
		// not implemented

	#elif AE_SIMD_NEON
		AE_INLINE_ALL
		GEMM_Neon::MatrixMulAdd( matA.Data().get(), matB.Data().get(), matC.Data().get(), M, N, K );

	#else
		// not implemented
	#endif
	}

/*
=================================================
	ReKU
=================================================
*/
namespace {
	template <typename SimdType>
	void  MatReLU (INOUT LargeMatrixView<float, EMatrixLayout::RowMajor> mat, float scalarScale) __NE___
	{
		const SimdType	scale	{scalarScale};
		const uint		M		= mat.Dimension().rows;
		const uint		K		= mat.Dimension().columns;
		const uint		K_step	= SimdType::count;

		for (uint m = 0; m < M; ++m)
		{
			float*	row = mat.Row( m ).Data();

			uint k = 0;
			for (; k + K_step < K; k += K_step)
			{
				SimdType	a0{ row + k };
				a0 = a0.Max( a0 * scale );
				a0.ToArray( OUT row + k );
			}

			for (; k < K; ++k)
			{
				float	a0 = row[k];
				a0 = Max( a0, a0 * scalarScale );
				row[k] = a0;
			}
		}
	}
}
	void  GEMM::ReLU (INOUT LargeMatrixView<float, EMatrixLayout::RowMajor> mat, float scale) __NE___
	{
		ASSERT( IsValid( mat ));

	  #if 0 //defined(AE_SIMD_SimdFloat16)
		// not implemented

	  #elif defined(AE_SIMD_SimdFloat8)
		MatReLU<SimdFloat8>( mat, scale );

	  #elif defined(AE_SIMD_SimdFloat4)
		MatReLU<SimdFloat4>( mat, scale );

	  #else
		// not implemented
	  #endif
	}

/*
=================================================
	MulAdd
=================================================
*/
	void  GEMV::MulAdd (LargeVectorView<const float>							vecA,
						LargeMatrixView<const float, EMatrixLayout::RowMajor>	matB,
						INOUT LargeVectorView<float>							vecC) __NE___
	{
		const uint	K = matB.Dimension().columns;
		const uint	M = matB.Dimension().rows;

		ASSERT( IsValid( vecA ));
		ASSERT( GEMM::IsValid( matB ));
		ASSERT( IsValid( vecC ));
		ASSERT( IsValidDim( vecA, matB, vecC ));

	#if 0 //AE_SIMD_AVX >= 30
		// not implemented

	#elif AE_SIMD_AVX >= 1
		AE_INLINE_ALL
		GEMV_AVX::MatVecMulAdd( vecA.Data().get(), matB.Data().get(), INOUT vecC.Data().get(), M, K );

	#elif AE_SIMD_SSE >= 20
		// not implemented

	#elif AE_SIMD_NEON
		AE_INLINE_ALL
		GEMV_Neon::MatVecMulAdd( vecA.Data().get(), matB.Data().get(), INOUT vecC.Data().get(), M, K );

	#else
		// not implemented
	#endif
	}

/*
=================================================
	ReLU
=================================================
*/
namespace {
	template <typename SimdType>
	void  VecReLU (INOUT LargeVectorView<float> vec, float scalarScale) __NE___
	{
		const SimdType	scale	{scalarScale};
		const uint		M		= vec.Length();
		const uint		M_step	= SimdType::count;
		float *			row		= vec.Data();

		uint m = 0;
		for (; m + M_step < M; m += M_step)
		{
			SimdType	a0{ row + m };
			a0 = a0.Max( a0 * scale );
			a0.ToArray( OUT row + m );
		}

		for (; m < M; ++m)
		{
			float	a0 = row[m];
			a0 = Max( a0, a0 * scalarScale );
			row[m] = a0;
		}
	}
}
	void  GEMV::ReLU (INOUT LargeVectorView<float> vec, float scale) __NE___
	{
		ASSERT( IsValid( vec ));

	  #if 0 //defined(AE_SIMD_SimdFloat16)
		// not implemented

	  #elif defined(AE_SIMD_SimdFloat8)
		VecReLU<SimdFloat8>( vec, scale );

	  #elif defined(AE_SIMD_SimdFloat4)
		VecReLU<SimdFloat4>( vec, scale );

	  #else
		// not implemented
	  #endif
	}

} // AE::Base
