// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Perf_Common.h"

#ifdef AE_COMPILER_MSVC
	#define RESTRICT __restrict
#else
	#define RESTRICT __restrict__
#endif

namespace
{
	template <typename T>
	void  FillWithLinearData (INOUT DynUntypedStorage &buf, float scale, float bias)
	{
		T*			arr		= buf.Ptr<T>();
		const usize	count	{buf.Size() / SizeOf<T>};

		for (usize i = 0; i < count; ++i) {
			arr[i] = T( float(i) * scale + bias );
		}
	}


	static void  TestVecMatMulAdd (ArrayView<float> inputA, ArrayView<float> inputB, ArrayView<float> inputC, ArrayView<float> refOutput,
								   const uint rowA, const MatrixDim dimB, const uint rowC, const bool columnMajor, StringView testName)
	{
		TEST( dimB.rows		== rowC );
		TEST( dimB.columns	== rowA );

		TEST( inputA.size() == rowA );
		TEST( inputB.size() == dimB.Size() );
		TEST( inputC.size() == rowC );
		TEST( refOutput.size() == rowC );

		Array<float>	output;
		output.resize( rowC );

		if ( columnMajor )
		{
			for (uint i = 0; i < rowC; ++i)
			{
				float	sum = float(inputC[i]);
				for (uint j = 0; j < rowA; ++j) {
					sum += float(inputB[ i + j * rowC ]) * float(inputA[j]);
				}
				output[i] = sum;
			}
		}
		else
		{
			for (uint i = 0; i < rowC; ++i)
			{
				float	sum = float(inputC[i]);
				for (uint j = 0; j < rowA; ++j) {
					sum += float(inputB[ i * rowA + j ]) * float(inputA[j]);
				}
				output[i] = sum;
			}
		}

		String	str;
		float	max_err	= 0.f;
		float	avr_err	= 0.f;

		//str << "\n| I | expected | SIMD output | error % |\n";

		for (uint i = 0; i < rowC; ++i)
		{
			float	err	= Abs(output[i] - refOutput[i]) * 100.f / Max( Abs(output[i]), 1.0e-5f );
			max_err = Max( max_err, err );
			avr_err += err;

		//	str << "| " << ToString( i ) << " | " << ToString( output[i], 5 )
		//		<< " | " << ToString( refOutput[i], 5 ) << " | "
		//		<< ToString( err, 2 ) << "% |\n";
		}
		avr_err /= float(rowC);

		str << testName << ": max error: " << ToString( max_err, 2 ) << "%, avr: " << ToString( avr_err, 2 ) << '%';
		//str << "\n----------------------------------------\n\n";
		AE_LOGI( str );
		CHECK( max_err < 1.f );
	}

	template <typename T>
	static void  TestVecMatMulAdd (const DynUntypedStorage &inputA, const DynUntypedStorage &inputB,
								   const DynUntypedStorage &inputC, const DynUntypedStorage &refOutput,
								   const uint rowA, const MatrixDim dimB, const uint rowC, const bool columnMajor,
								   StringView testName)
	{
		return TestVecMatMulAdd( ArrayView<T>{ inputA.Ptr<T>(), rowA },
								 ArrayView<T>{ inputB.Ptr<T>(), dimB.Size() },
								 ArrayView<T>{ inputC.Ptr<T>(), rowC },
								 ArrayView<T>{ refOutput.Ptr<T>(), rowC },
								 rowA, dimB, rowC, columnMajor, testName );
	}
//-----------------------------------------------------------------------------



#if AE_SIMD_AVX >= 1
namespace avx_v1
{
	static void  MatVecMulAdd (const float*	RESTRICT	A,	// A[K]
							   const float*	RESTRICT	B,	// B[MxK]
							   INOUT float*	RESTRICT	C,	// C[M]
							   const uint				M,
							   const uint				K) __NE___
	{
		for (uint m = 0; m < M; ++m)
		{
			const float* b = B + m * K;

			__m256 sum0 = _mm256_setzero_ps();
			__m256 sum1 = _mm256_setzero_ps();
			__m256 sum2 = _mm256_setzero_ps();
			__m256 sum3 = _mm256_setzero_ps();

			// 4 independent accumulation chains = better ILP.
			uint k = 0;
			for (; k + 32 <= K; k += 32)
			{
				const __m256 a0 = _mm256_load_ps( A + k +  0 );
				const __m256 a1 = _mm256_load_ps( A + k +  8 );
				const __m256 a2 = _mm256_load_ps( A + k + 16 );
				const __m256 a3 = _mm256_load_ps( A + k + 24 );

				const __m256 b0 = _mm256_load_ps( b + k +  0 );
				const __m256 b1 = _mm256_load_ps( b + k +  8 );
				const __m256 b2 = _mm256_load_ps( b + k + 16 );
				const __m256 b3 = _mm256_load_ps( b + k + 24 );

			  #if AE_SIMD_FMA
				sum0 = _mm256_fmadd_ps( a0, b0, sum0 );
				sum1 = _mm256_fmadd_ps( a1, b1, sum1 );
				sum2 = _mm256_fmadd_ps( a2, b2, sum2 );
				sum3 = _mm256_fmadd_ps( a3, b3, sum3 );
			  #else
				sum0 = _mm256_add_ps( sum0, _mm256_mul_ps( a0, b0 ));
				sum1 = _mm256_add_ps( sum1, _mm256_mul_ps( a1, b1 ));
				sum2 = _mm256_add_ps( sum2, _mm256_mul_ps( a2, b2 ));
				sum3 = _mm256_add_ps( sum3, _mm256_mul_ps( a3, b3 ));
			  #endif
			}

			for (; k + 8 <= K; k += 8)
			{
				const __m256 av = _mm256_loadu_ps( A + k );
				const __m256 bv = _mm256_loadu_ps( b + k );

			  #if AE_SIMD_FMA
				sum0 = _mm256_fmadd_ps( av, bv, sum0 );
			  #else
				sum0 = _mm256_add_ps( sum0, _mm256_mul_ps( av, bv ));
			  #endif
			}

			sum0 = _mm256_add_ps( sum0, sum1 );
			sum2 = _mm256_add_ps( sum2, sum3 );
			sum0 = _mm256_add_ps( sum0, sum2 );

			__m128	lo		= _mm256_castps256_ps128( sum0 );
			__m128	hi		= _mm256_extractf128_ps( sum0, 1 );
			__m128	sum128	= _mm_add_ps( lo, hi );

			sum128 = _mm_hadd_ps( sum128, sum128 );
			sum128 = _mm_hadd_ps( sum128, sum128 );

			float sum = _mm_cvtss_f32( sum128 );

			// tail < 8
			for (; k < K; ++k) {
				sum += A[k] * b[k];
			}

			C[m] += sum;
		}
	}

} // avx_v1
#endif // AVX
//-----------------------------------------------------------------------------


#if AE_SIMD_AVX >= 1
namespace avx_v2
{
	static inline __m128  HSum4x8 (const __m256 a0, const __m256 a1,
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
							   const uint M, const uint K) __NE___
	{
		uint m = 0;
		for (; m + 4 <= M; m += 4)
		{
			const float* RESTRICT b0 = B + (m + 0) * K;
			const float* RESTRICT b1 = B + (m + 1) * K;
			const float* RESTRICT b2 = B + (m + 2) * K;
			const float* RESTRICT b3 = B + (m + 3) * K;

			__m256 acc00 = _mm256_setzero_ps(), acc01 = _mm256_setzero_ps();
			__m256 acc10 = _mm256_setzero_ps(), acc11 = _mm256_setzero_ps();
			__m256 acc20 = _mm256_setzero_ps(), acc21 = _mm256_setzero_ps();
			__m256 acc30 = _mm256_setzero_ps(), acc31 = _mm256_setzero_ps();

			uint k = 0;
			for (; k + 16 <= K; k += 16)
			{
				__m256 a0 = _mm256_loadu_ps( A + k + 0 );
				__m256 a1 = _mm256_loadu_ps( A + k + 8 );

				acc00 = _mm256_fmadd_ps( a0, _mm256_loadu_ps( b0 + k + 0 ), acc00 );
				acc01 = _mm256_fmadd_ps( a1, _mm256_loadu_ps( b0 + k + 8 ), acc01 );
				acc10 = _mm256_fmadd_ps( a0, _mm256_loadu_ps( b1 + k + 0 ), acc10 );
				acc11 = _mm256_fmadd_ps( a1, _mm256_loadu_ps( b1 + k + 8 ), acc11 );
				acc20 = _mm256_fmadd_ps( a0, _mm256_loadu_ps( b2 + k + 0 ), acc20 );
				acc21 = _mm256_fmadd_ps( a1, _mm256_loadu_ps( b2 + k + 8 ), acc21 );
				acc30 = _mm256_fmadd_ps( a0, _mm256_loadu_ps( b3 + k + 0 ), acc30 );
				acc31 = _mm256_fmadd_ps( a1, _mm256_loadu_ps( b3 + k + 8 ), acc31 );
			}

			if ( k + 8 <= K )
			{
				__m256 a0 = _mm256_loadu_ps( A + k );
				acc00 = _mm256_fmadd_ps( a0, _mm256_loadu_ps( b0 + k ), acc00 );
				acc10 = _mm256_fmadd_ps( a0, _mm256_loadu_ps( b1 + k ), acc10 );
				acc20 = _mm256_fmadd_ps( a0, _mm256_loadu_ps( b2 + k ), acc20 );
				acc30 = _mm256_fmadd_ps( a0, _mm256_loadu_ps( b3 + k ), acc30 );
				k += 8;
			}

			acc00 = _mm256_add_ps( acc00, acc01 );
			acc10 = _mm256_add_ps( acc10, acc11 );
			acc20 = _mm256_add_ps( acc20, acc21 );
			acc30 = _mm256_add_ps( acc30, acc31 );

			float t[4];
			_mm_storeu_ps( t, HSum4x8( acc00, acc10, acc20, acc30 ));

			for (; k < K; ++k)   // scalar tail
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

		for (; m < M; ++m)   // row tail: single-row dot product, 4 accumulators
		{
			const float* RESTRICT b = B + m * K;
			__m256 acc0 = _mm256_setzero_ps(), acc1 = _mm256_setzero_ps();
			__m256 acc2 = _mm256_setzero_ps(), acc3 = _mm256_setzero_ps();

			uint k = 0;
			for (; k + 32 <= K; k += 32)
			{
				acc0 = _mm256_fmadd_ps( _mm256_loadu_ps( A+k+ 0 ), _mm256_loadu_ps( b+k+ 0 ), acc0 );
				acc1 = _mm256_fmadd_ps( _mm256_loadu_ps( A+k+ 8 ), _mm256_loadu_ps( b+k+ 8 ), acc1 );
				acc2 = _mm256_fmadd_ps( _mm256_loadu_ps( A+k+16 ), _mm256_loadu_ps( b+k+16 ), acc2 );
				acc3 = _mm256_fmadd_ps( _mm256_loadu_ps( A+k+24 ), _mm256_loadu_ps( b+k+24 ), acc3 );
			}

			for (; k + 8 <= K; k += 8)
			{
				acc0 = _mm256_fmadd_ps( _mm256_loadu_ps( A+k ), _mm256_loadu_ps( b+k ), acc0 );
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

} // avx_v2
#endif // AVX
//-----------------------------------------------------------------------------


#if AE_SIMD_AVX >= 1
namespace avx_v3
{
	static forceinline float HorizontalSum(__m256 v) __NE___
	{
		__m128 lo = _mm256_castps256_ps128(v);
		__m128 hi = _mm256_extractf128_ps(v, 1);
		__m128 s  = _mm_add_ps(lo, hi);
		s = _mm_hadd_ps(s, s);
		s = _mm_hadd_ps(s, s);
		return _mm_cvtss_f32(s);
	}

	static void MatVecMulAdd(
		const float* RESTRICT A,  // A[K]
		const float* RESTRICT B,  // B[M x K], row-major
		INOUT float* RESTRICT C,  // C[M]
		const uint M,
		const uint K) __NE___
	{
		const uint M8 = M - (M % 8);
		const uint K8 = K - (K % 8);

		for (uint i = 0; i < M8; i += 8)
		{
			const float* b0 = B + (i + 0) * K;
			const float* b1 = B + (i + 1) * K;
			const float* b2 = B + (i + 2) * K;
			const float* b3 = B + (i + 3) * K;
			const float* b4 = B + (i + 4) * K;
			const float* b5 = B + (i + 5) * K;
			const float* b6 = B + (i + 6) * K;
			const float* b7 = B + (i + 7) * K;

			__m256 s0 = _mm256_setzero_ps();
			__m256 s1 = _mm256_setzero_ps();
			__m256 s2 = _mm256_setzero_ps();
			__m256 s3 = _mm256_setzero_ps();
			__m256 s4 = _mm256_setzero_ps();
			__m256 s5 = _mm256_setzero_ps();
			__m256 s6 = _mm256_setzero_ps();
			__m256 s7 = _mm256_setzero_ps();

			uint k = 0;

			for (; k < K8; k += 8)
			{
				__m256 a = _mm256_loadu_ps(A + k);

			#if AE_SIMD_FMA
				s0 = _mm256_fmadd_ps(a, _mm256_loadu_ps(b0 + k), s0);
				s1 = _mm256_fmadd_ps(a, _mm256_loadu_ps(b1 + k), s1);
				s2 = _mm256_fmadd_ps(a, _mm256_loadu_ps(b2 + k), s2);
				s3 = _mm256_fmadd_ps(a, _mm256_loadu_ps(b3 + k), s3);
				s4 = _mm256_fmadd_ps(a, _mm256_loadu_ps(b4 + k), s4);
				s5 = _mm256_fmadd_ps(a, _mm256_loadu_ps(b5 + k), s5);
				s6 = _mm256_fmadd_ps(a, _mm256_loadu_ps(b6 + k), s6);
				s7 = _mm256_fmadd_ps(a, _mm256_loadu_ps(b7 + k), s7);
			#else
				s0 = _mm256_add_ps(_mm256_mul_ps(a, _mm256_loadu_ps(b0 + k)), s0);
				s1 = _mm256_add_ps(_mm256_mul_ps(a, _mm256_loadu_ps(b1 + k)), s1);
				s2 = _mm256_add_ps(_mm256_mul_ps(a, _mm256_loadu_ps(b2 + k)), s2);
				s3 = _mm256_add_ps(_mm256_mul_ps(a, _mm256_loadu_ps(b3 + k)), s3);
				s4 = _mm256_add_ps(_mm256_mul_ps(a, _mm256_loadu_ps(b4 + k)), s4);
				s5 = _mm256_add_ps(_mm256_mul_ps(a, _mm256_loadu_ps(b5 + k)), s5);
				s6 = _mm256_add_ps(_mm256_mul_ps(a, _mm256_loadu_ps(b6 + k)), s6);
				s7 = _mm256_add_ps(_mm256_mul_ps(a, _mm256_loadu_ps(b7 + k)), s7);
			#endif
			}

			float r0 = HorizontalSum(s0);
			float r1 = HorizontalSum(s1);
			float r2 = HorizontalSum(s2);
			float r3 = HorizontalSum(s3);
			float r4 = HorizontalSum(s4);
			float r5 = HorizontalSum(s5);
			float r6 = HorizontalSum(s6);
			float r7 = HorizontalSum(s7);

			for (; k < K; ++k)
			{
				const float a = A[k];

				r0 += a * b0[k];
				r1 += a * b1[k];
				r2 += a * b2[k];
				r3 += a * b3[k];
				r4 += a * b4[k];
				r5 += a * b5[k];
				r6 += a * b6[k];
				r7 += a * b7[k];
			}

			C[i + 0] += r0;
			C[i + 1] += r1;
			C[i + 2] += r2;
			C[i + 3] += r3;
			C[i + 4] += r4;
			C[i + 5] += r5;
			C[i + 6] += r6;
			C[i + 7] += r7;
		}

		for (uint i = M8; i < M; ++i)
		{
			const float* b = B + i * K;

			__m256 sum = _mm256_setzero_ps();

			uint k = 0;
			for (; k < K8; k += 8)
			{
				__m256 a  = _mm256_loadu_ps(A + k);
				__m256 bv = _mm256_loadu_ps(b + k);

			#if AE_SIMD_FMA
				sum = _mm256_fmadd_ps(a, bv, sum);
			#else
				sum = _mm256_add_ps(_mm256_mul_ps(a, bv), sum);
			#endif
			}

			float r = HorizontalSum(sum);

			for (; k < K; ++k)
				r += A[k] * b[k];

			C[i] += r;
		}
	}

} // avx_v3
#endif // AVX
//-----------------------------------------------------------------------------


#if AE_SIMD_AVX >= 1
namespace avx_v4
{
	static void MatVecMulAdd (const float* RESTRICT A,
							  const float* RESTRICT B,
							  INOUT float* RESTRICT C,
							  const uint M,
							  const uint K) __NE___
	{
		for (uint m = 0; m < M; ++m)
		{
			// Pointer to current row of B
			const float* b = B + m * K;

			// Prefetch next rows of B to hide memory latency.
			// Fetching ahead by ~4 rows is usually optimal for L2/L3 cache.
			//if (m + 4 < M) {
			//	__builtin_prefetch(B + (m + 4) * K, 0, 1);
			//}

			// Initialize accumulators with zero
			__m256 sum0 = _mm256_setzero_ps();
			__m256 sum1 = _mm256_setzero_ps();
			__m256 sum2 = _mm256_setzero_ps();
			__m256 sum3 = _mm256_setzero_ps();

			uint k = 0;

			// Main Loop: Process 32 elements (4 chains of 8) per iteration for ILP
			// Assumes A and B are 32-byte aligned. If not, use _mm256_loadu_ps.
			for (; k + 32 <= K; k += 32)
			{
				const __m256 a0 = _mm256_load_ps(A + k + 0);
				const __m256 a1 = _mm256_load_ps(A + k + 8);
				const __m256 a2 = _mm256_load_ps(A + k + 16);
				const __m256 a3 = _mm256_load_ps(A + k + 24);

				const __m256 b0 = _mm256_load_ps(b + k + 0);
				const __m256 b1 = _mm256_load_ps(b + k + 8);
				const __m256 b2 = _mm256_load_ps(b + k + 16);
				const __m256 b3 = _mm256_load_ps(b + k + 24);

			  #if AE_SIMD_FMA
				sum0 = _mm256_fmadd_ps(a0, b0, sum0);
				sum1 = _mm256_fmadd_ps(a1, b1, sum1);
				sum2 = _mm256_fmadd_ps(a2, b2, sum2);
				sum3 = _mm256_fmadd_ps(a3, b3, sum3);
			  #else
				sum0 = _mm256_add_ps(sum0, _mm256_mul_ps(a0, b0));
				sum1 = _mm256_add_ps(sum1, _mm256_mul_ps(a1, b1));
				sum2 = _mm256_add_ps(sum2, _mm256_mul_ps(a2, b2));
				sum3 = _mm256_add_ps(sum3, _mm256_mul_ps(a3, b3));
			  #endif
			}

			// Tail Loop 1: Handle remainder in chunks of 8 (SIMD)
			for (; k + 8 <= K; k += 8)
			{
				const __m256 av = _mm256_loadu_ps(A + k); // Use loadu for safety on tail
				const __m256 bv = _mm256_loadu_ps(b + k);

			  #if AE_SIMD_FMA
				sum0 = _mm256_fmadd_ps(av, bv, sum0);
			  #else
				sum0 = _mm256_add_ps(sum0, _mm256_mul_ps(av, bv));
			  #endif
			}

			// Combine the 4 independent chains into one accumulator (sum0)
			sum0 = _mm256_add_ps(sum0, sum1);
			sum2 = _mm256_add_ps(sum2, sum3);
			sum0 = _mm256_add_ps(sum0, sum2);

			// Horizontal Reduction: Reduce 8 floats to a scalar
			__m128 lo = _mm256_castps256_ps128(sum0);
			__m128 hi = _mm256_extractf128_ps(sum0, 1);
			__m128 sum128 = _mm_add_ps(lo, hi); // Now has 4 floats

			// Reduce 4 floats -> 2 floats -> 1 float
			sum128 = _mm_hadd_ps(sum128, sum128);
			sum128 = _mm_hadd_ps(sum128, sum128);

			float sum = _mm_cvtss_f32(sum128);

			// Tail Loop 2: Handle remainder < 8 (Scalar)
			for (; k < K; ++k) {
				sum += A[k] * b[k];
			}

			C[m] += sum;
		}
	}

} // avx_v4
#endif // AVX
//-----------------------------------------------------------------------------


#if AE_SIMD_NEON
namespace neon_v1
{
	// -> [ hsum(a0), hsum(a1), hsum(a2), hsum(a3) ]
	// each 'aX' is 8 floats: aXl = elements 0..3, aXh = elements 4..7
	inline float32x4_t  HSum4x8 (const float32x4_t a0l, const float32x4_t a0h,
								 const float32x4_t a1l, const float32x4_t a1h,
								 const float32x4_t a2l, const float32x4_t a2h,
								 const float32x4_t a3l, const float32x4_t a3h) __NE___
	{
		const float32x4_t	s0 = vaddq_f32( a0l, a0h );
		const float32x4_t	s1 = vaddq_f32( a1l, a1h );
		const float32x4_t	s2 = vaddq_f32( a2l, a2h );
		const float32x4_t	s3 = vaddq_f32( a3l, a3h );

		const float32x4_t	t0 = vpaddq_f32( s0, s1 );	// [s0.01, s0.23, s1.01, s1.23]
		const float32x4_t	t1 = vpaddq_f32( s2, s3 );	// [s2.01, s2.23, s3.01, s3.23]
		return vpaddq_f32( t0, t1 );					// [hsum(a0), hsum(a1), hsum(a2), hsum(a3)]
	}

	static void  MatVecMulAdd (const float* RESTRICT A,   // A[K]
							   const float* RESTRICT B,   // B[M][K]
							   INOUT float* RESTRICT C,   // C[M]
							   const uint			 M,
							   const uint			 K) __NE___
	{
		const float32x4_t	zero = vdupq_n_f32( 0.0f );

		uint m = 0;
		for (; m + 4 <= M; m += 4)
		{
			const float* RESTRICT	b0 = B + (m + 0) * K;
			const float* RESTRICT	b1 = B + (m + 1) * K;
			const float* RESTRICT	b2 = B + (m + 2) * K;
			const float* RESTRICT	b3 = B + (m + 3) * K;

			float32x4_t acc00 = zero, acc01 = zero, acc02 = zero, acc03 = zero;
			float32x4_t acc10 = zero, acc11 = zero, acc12 = zero, acc13 = zero;
			float32x4_t acc20 = zero, acc21 = zero, acc22 = zero, acc23 = zero;
			float32x4_t acc30 = zero, acc31 = zero, acc32 = zero, acc33 = zero;

			uint k = 0;
			for (; k + 16 <= K; k += 16)
			{
				const float32x4_t	a0 = vld1q_f32( A + k +  0 );
				const float32x4_t	a1 = vld1q_f32( A + k +  4 );
				const float32x4_t	a2 = vld1q_f32( A + k +  8 );
				const float32x4_t	a3 = vld1q_f32( A + k + 12 );

				acc00 = vfmaq_f32( acc00, a0, vld1q_f32( b0 + k +  0 ) );
				acc01 = vfmaq_f32( acc01, a1, vld1q_f32( b0 + k +  4 ) );
				acc02 = vfmaq_f32( acc02, a2, vld1q_f32( b0 + k +  8 ) );
				acc03 = vfmaq_f32( acc03, a3, vld1q_f32( b0 + k + 12 ) );
				acc10 = vfmaq_f32( acc10, a0, vld1q_f32( b1 + k +  0 ) );
				acc11 = vfmaq_f32( acc11, a1, vld1q_f32( b1 + k +  4 ) );
				acc12 = vfmaq_f32( acc12, a2, vld1q_f32( b1 + k +  8 ) );
				acc13 = vfmaq_f32( acc13, a3, vld1q_f32( b1 + k + 12 ) );
				acc20 = vfmaq_f32( acc20, a0, vld1q_f32( b2 + k +  0 ) );
				acc21 = vfmaq_f32( acc21, a1, vld1q_f32( b2 + k +  4 ) );
				acc22 = vfmaq_f32( acc22, a2, vld1q_f32( b2 + k +  8 ) );
				acc23 = vfmaq_f32( acc23, a3, vld1q_f32( b2 + k + 12 ) );
				acc30 = vfmaq_f32( acc30, a0, vld1q_f32( b3 + k +  0 ) );
				acc31 = vfmaq_f32( acc31, a1, vld1q_f32( b3 + k +  4 ) );
				acc32 = vfmaq_f32( acc32, a2, vld1q_f32( b3 + k +  8 ) );
				acc33 = vfmaq_f32( acc33, a3, vld1q_f32( b3 + k + 12 ) );
			}

			if ( k + 8 <= K )
			{
				const float32x4_t	a0 = vld1q_f32( A + k + 0 );
				const float32x4_t	a1 = vld1q_f32( A + k + 4 );

				acc00 = vfmaq_f32( acc00, a0, vld1q_f32( b0 + k + 0 ) );
				acc01 = vfmaq_f32( acc01, a1, vld1q_f32( b0 + k + 4 ) );
				acc10 = vfmaq_f32( acc10, a0, vld1q_f32( b1 + k + 0 ) );
				acc11 = vfmaq_f32( acc11, a1, vld1q_f32( b1 + k + 4 ) );
				acc20 = vfmaq_f32( acc20, a0, vld1q_f32( b2 + k + 0 ) );
				acc21 = vfmaq_f32( acc21, a1, vld1q_f32( b2 + k + 4 ) );
				acc30 = vfmaq_f32( acc30, a0, vld1q_f32( b3 + k + 0 ) );
				acc31 = vfmaq_f32( acc31, a1, vld1q_f32( b3 + k + 4 ) );
				k += 8;
			}

			if ( k + 4 <= K )	// NEON is 4-wide: one extra vector step
			{
				const float32x4_t	a0 = vld1q_f32( A + k );

				acc00 = vfmaq_f32( acc00, a0, vld1q_f32( b0 + k ) );
				acc10 = vfmaq_f32( acc10, a0, vld1q_f32( b1 + k ) );
				acc20 = vfmaq_f32( acc20, a0, vld1q_f32( b2 + k ) );
				acc30 = vfmaq_f32( acc30, a0, vld1q_f32( b3 + k ) );
				k += 4;
			}

			// fold 4 accumulators per row down to 8 floats (lo/hi pair)
			const float32x4_t	s0l = vaddq_f32( acc00, acc01 );
			const float32x4_t	s0h = vaddq_f32( acc02, acc03 );
			const float32x4_t	s1l = vaddq_f32( acc10, acc11 );
			const float32x4_t	s1h = vaddq_f32( acc12, acc13 );
			const float32x4_t	s2l = vaddq_f32( acc20, acc21 );
			const float32x4_t	s2h = vaddq_f32( acc22, acc23 );
			const float32x4_t	s3l = vaddq_f32( acc30, acc31 );
			const float32x4_t	s3h = vaddq_f32( acc32, acc33 );

			float t[4];		// stack only, no heap
			vst1q_f32( t, HSum4x8( s0l, s0h, s1l, s1h, s2l, s2h, s3l, s3h ));

			// scalar tail
			for (; k < K; ++k)
			{
				const float	a = A[k];
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

		// row tail: single-row dot product, 8 accumulators (32 floats/iter)
		for (; m < M; ++m)
		{
			const float* RESTRICT	b = B + m * K;

			float32x4_t acc0 = zero, acc1 = zero, acc2 = zero, acc3 = zero;
			float32x4_t acc4 = zero, acc5 = zero, acc6 = zero, acc7 = zero;

			uint k = 0;
			for (; k + 32 <= K; k += 32)
			{
				acc0 = vfmaq_f32( acc0, vld1q_f32( A+k+ 0 ), vld1q_f32( b+k+ 0 ) );
				acc1 = vfmaq_f32( acc1, vld1q_f32( A+k+ 4 ), vld1q_f32( b+k+ 4 ) );
				acc2 = vfmaq_f32( acc2, vld1q_f32( A+k+ 8 ), vld1q_f32( b+k+ 8 ) );
				acc3 = vfmaq_f32( acc3, vld1q_f32( A+k+12 ), vld1q_f32( b+k+12 ) );
				acc4 = vfmaq_f32( acc4, vld1q_f32( A+k+16 ), vld1q_f32( b+k+16 ) );
				acc5 = vfmaq_f32( acc5, vld1q_f32( A+k+20 ), vld1q_f32( b+k+20 ) );
				acc6 = vfmaq_f32( acc6, vld1q_f32( A+k+24 ), vld1q_f32( b+k+24 ) );
				acc7 = vfmaq_f32( acc7, vld1q_f32( A+k+28 ), vld1q_f32( b+k+28 ) );
			}

			for (; k + 4 <= K; k += 4)
			{
				acc0 = vfmaq_f32( acc0, vld1q_f32( A+k ), vld1q_f32( b+k ) );
			}

			acc0 = vaddq_f32( vaddq_f32( acc0, acc1 ), vaddq_f32( acc2, acc3 ) );
			acc4 = vaddq_f32( vaddq_f32( acc4, acc5 ), vaddq_f32( acc6, acc7 ) );
			acc0 = vaddq_f32( acc0, acc4 );

			float	sum = vaddvq_f32( acc0 );	// horizontal sum (AArch64)

			for (; k < K; ++k)
			{
				sum += b[k] * A[k];
			}
			C[m] += sum;
		}
	}

} // neon_v1
#endif // NEON
//-----------------------------------------------------------------------------


#if AE_SIMD_NEON
namespace neon_v2
{
	/*
	 * A : [K]
	 * B : [M][K]   (row‑major)
	 * C : [M]      (input = old value, output = old + dot(A,B[i]))
	 *
	 * The routine is fully vectorised over the K dimension.  For each row we
	 * accumulate a 128‑bit register and finally reduce it to a scalar.
	 */
	static void MatVecMulAdd(const float * RESTRICT A,
							 const float * RESTRICT B,
							 float       * RESTRICT C,
							 const unsigned M,
							 const unsigned K) __NE___
	{
		for (unsigned i = 0; i < M; ++i)
		{
			const float *brow = B + i*K;          // start of row i in B
			unsigned k = 0;
			float32x4_t acc = vdupq_n_f32(0.0f);   // zero accumulator

			/*--- 4‑element blocks ------------------------------------------------*/
			for (; k + 3 < K; k += 4)
			{
				const float32x4_t a_vec = vld1q_f32(A + k);
				const float32x4_t b_vec = vld1q_f32(brow + k);

				/* acc += a_vec * b_vec   (vector multiply‑add) */
				acc = vfmaq_f32(acc, b_vec, a_vec);  // or: acc = vmlaq_f32(acc,a_vec,b_vec);
			}

			/*--- Horizontal sum of the vector accumulator ---------------------*/
		#if defined(__ARM_FEATURE_DOTPROD)
			/* ARMv8.2+ DP extension – single instruction that sums all lanes */
			float sum = vaddvq_f32(acc);   // scalar
		#else
			/* Portable reduction: pair‑wise add low/high halves, then the two results */
			const float32x2_t tmp  = vpadd_f32(vget_low_f32(acc),  vget_high_f32(acc));
			float sum = vget_lane_f32(tmp, 0) + vget_lane_f32(tmp, 1);
		#endif

			/*--- Tail (remaining <4 elements) -------------------------------------*/
			for (; k < K; ++k)
				sum += A[k] * brow[k];

			/*--- Add the accumulated dot product to the original C[i] ------------*/
			C[i] += sum;
		}
	}

} // neon_v2
#endif // NEON
//-----------------------------------------------------------------------------


#if AE_SIMD_NEON
namespace neon_v3
{
	static void MatVecMulAdd(const float* RESTRICT A,   // A[K]
							 const float* RESTRICT B,   // B[M][K]
							 INOUT float* RESTRICT C,   // C[M]
							 const uint32_t        M,
							 const uint32_t        K) __NE___
	{
		uint32_t i = 0;

		// Unroll outer loop by 4 to compute 4 rows of C simultaneously.
		// This dramatically improves pipeline utilization and register reuse.
		for (; i + 3 < M; i += 4)
		{
			// Pointers to the start of 4 consecutive rows in B
			const float* rowB0 = B + (i + 0) * K;
			const float* rowB1 = B + (i + 1) * K;
			const float* rowB2 = B + (i + 2) * K;
			const float* rowB3 = B + (i + 3) * K;

			// Load current C elements into accumulators
			float32x4_t acc0 = vdupq_n_f32(C[i + 0]);
			float32x4_t acc1 = vdupq_n_f32(C[i + 1]);
			float32x4_t acc2 = vdupq_n_f32(C[i + 2]);
			float32x4_t acc3 = vdupq_n_f32(C[i + 3]);

			uint32_t j = 0;
			// Inner loop: Process vector A and rows of B 4 elements at a time
			for (; j + 3 < K; j += 4)
			{
				float32x4_t vecA = vld1q_f32(A + j);

				float32x4_t vecB0 = vld1q_f32(rowB0 + j);
				float32x4_t vecB1 = vld1q_f32(rowB1 + j);
				float32x4_t vecB2 = vld1q_f32(rowB2 + j);
				float32x4_t vecB3 = vld1q_f32(rowB3 + j);

				// Multiply and accumulate into the 4 horizontal vector accumulators
				acc0 = vfmaq_f32(acc0, vecA, vecB0);
				acc1 = vfmaq_f32(acc1, vecA, vecB1);
				acc2 = vfmaq_f32(acc2, vecA, vecB2);
				acc3 = vfmaq_f32(acc3, vecA, vecB3);
			}

			// Reduce the 4-element vectors horizontally into single scalars
			C[i + 0] = vaddvq_f32(acc0);
			C[i + 1] = vaddvq_f32(acc1);
			C[i + 2] = vaddvq_f32(acc2);
			C[i + 3] = vaddvq_f32(acc3);

			// Clean up remaining K elements for these 4 rows if K is not a multiple of 4
			for (; j < K; ++j) {
				float valA = A[j];
				C[i + 0] += valA * rowB0[j];
				C[i + 1] += valA * rowB1[j];
				C[i + 2] += valA * rowB2[j];
				C[i + 3] += valA * rowB3[j];
			}
		}

		// Clean up remaining rows if M is not a multiple of 4
		for (; i < M; ++i)
		{
			const float* rowB = B + i * K;
			float32x4_t acc = vdupq_n_f32(0.0f);

			uint32_t j = 0;
			for (; j + 3 < K; j += 4)
			{
				float32x4_t vecA = vld1q_f32(A + j);
				float32x4_t vecB = vld1q_f32(rowB + j);
				acc = vfmaq_f32(acc, vecA, vecB);
			}

			C[i] += vaddvq_f32(acc);

			// Clean up remaining elements for the final row edge-case
			for (; j < K; ++j) {
				C[i] += A[j] * rowB[j];
			}
		}
	}

} // neon_v3
#endif // NEON
//-----------------------------------------------------------------------------


#if AE_SIMD_NEON
namespace neon_v4
{
	static void MatVecMulAdd(const float* RESTRICT A,   // A[K]
							 const float* RESTRICT B,   // B[M][K]
							 INOUT float* RESTRICT C,   // C[M]
							 const uint32_t        M,
							 const uint32_t        K) __NE___
	{
		// Prefetch distance configuration (adjust based on target ARM CPU hardware profile)
		constexpr uint PREFETCH_AHEAD_BYTES = 128;

		// Ensure compiler knows pointers are well-aligned (essential for speed)
		A = (const float*)__builtin_assume_aligned(A, 16);
		B = (const float*)__builtin_assume_aligned(B, 16);
		C = (float*)__builtin_assume_aligned(C, 16);

		uint32_t i = 0;

		// Process 4 rows simultaneously to maximize register file usage
		for (; i + 3 < M; i += 4) {
			const float* RESTRICT rowB0 = B + (i + 0) * K;
			const float* RESTRICT rowB1 = B + (i + 1) * K;
			const float* RESTRICT rowB2 = B + (i + 2) * K;
			const float* RESTRICT rowB3 = B + (i + 3) * K;

			// Initialize Neon accumulators with zero
			float32x4_t acc0 = vdupq_n_f32(0.0f);
			float32x4_t acc1 = vdupq_n_f32(0.0f);
			float32x4_t acc2 = vdupq_n_f32(0.0f);
			float32x4_t acc3 = vdupq_n_f32(0.0f);

			uint32_t j = 0;

			// Inner loop unrolled to process 8 elements (2 Neon vectors) per iteration
			// This hides memory instruction latency by interleaving compute and loads
			for (; j + 7 < K; j += 8) {
				// Software Prefetching: Pull next data chunks into L1 cache ahead of time
				__builtin_prefetch(rowB0 + j + (PREFETCH_AHEAD_BYTES / sizeof(float)), 0, 3);
				__builtin_prefetch(rowB1 + j + (PREFETCH_AHEAD_BYTES / sizeof(float)), 0, 3);
				__builtin_prefetch(rowB2 + j + (PREFETCH_AHEAD_BYTES / sizeof(float)), 0, 3);
				__builtin_prefetch(rowB3 + j + (PREFETCH_AHEAD_BYTES / sizeof(float)), 0, 3);
				__builtin_prefetch(A + j + (PREFETCH_AHEAD_BYTES / sizeof(float)), 0, 3);

				// Load Chunk 1 (Elements 0-3)
				float32x4_t vecA_0 = vld1q_f32(A + j);
				float32x4_t vecB0_0 = vld1q_f32(rowB0 + j);
				float32x4_t vecB1_0 = vld1q_f32(rowB1 + j);
				float32x4_t vecB2_0 = vld1q_f32(rowB2 + j);
				float32x4_t vecB3_0 = vld1q_f32(rowB3 + j);

				// Compute Chunk 1
				acc0 = vfmaq_f32(acc0, vecA_0, vecB0_0);
				acc1 = vfmaq_f32(acc1, vecA_0, vecB1_0);
				acc2 = vfmaq_f32(acc2, vecA_0, vecB2_0);
				acc3 = vfmaq_f32(acc3, vecA_0, vecB3_0);

				// Load Chunk 2 (Elements 4-7)
				float32x4_t vecA_1 = vld1q_f32(A + j + 4);
				float32x4_t vecB0_1 = vld1q_f32(rowB0 + j + 4);
				float32x4_t vecB1_1 = vld1q_f32(rowB1 + j + 4);
				float32x4_t vecB2_1 = vld1q_f32(rowB2 + j + 4);
				float32x4_t vecB3_1 = vld1q_f32(rowB3 + j + 4);

				// Compute Chunk 2
				acc0 = vfmaq_f32(acc0, vecA_1, vecB0_1);
				acc1 = vfmaq_f32(acc1, vecA_1, vecB1_1);
				acc2 = vfmaq_f32(acc2, vecA_1, vecB2_1);
				acc3 = vfmaq_f32(acc3, vecA_1, vecB3_1);
			}

			// Handle remaining blocks of 4 elements if K is not a multiple of 8
			for (; j + 3 < K; j += 4) {
				float32x4_t vecA = vld1q_f32(A + j);
				acc0 = vfmaq_f32(acc0, vecA, vld1q_f32(rowB0 + j));
				acc1 = vfmaq_f32(acc1, vecA, vld1q_f32(rowB1 + j));
				acc2 = vfmaq_f32(acc2, vecA, vld1q_f32(rowB2 + j));
				acc3 = vfmaq_f32(acc3, vecA, vld1q_f32(rowB3 + j));
			}

			// Horizontal addition + accumulate directly into target vector C
			C[i + 0] += vaddvq_f32(acc0);
			C[i + 1] += vaddvq_f32(acc1);
			C[i + 2] += vaddvq_f32(acc2);
			C[i + 3] += vaddvq_f32(acc3);

			// Scalar cleanup for final tail elements of K
			for (; j < K; ++j) {
				float valA = A[j];
				C[i + 0] += valA * rowB0[j];
				C[i + 1] += valA * rowB1[j];
				C[i + 2] += valA * rowB2[j];
				C[i + 3] += valA * rowB3[j];
			}
		}

		// Scalar cleanup loop for remaining uneven rows of M
		for (; i < M; ++i) {
			const float* RESTRICT rowB = B + i * K;
			float32x4_t acc = vdupq_n_f32(0.0f);
			uint32_t j = 0;
			for (; j + 3 < K; j += 4) {
				acc = vfmaq_f32(acc, vld1q_f32(A + j), vld1q_f32(rowB + j));
			}
			C[i] += vaddvq_f32(acc);
			for (; j < K; ++j) {
				C[i] += A[j] * rowB[j];
			}
		}
	}
} // neon_v4
#endif // NEON
//-----------------------------------------------------------------------------


	static void  GEMVTest (const uint M, const uint K, const usize max_iter, ECoreType coreType)
	{
		const bool	col_maj		= false;

		const usize	n_muls		= M * K;
		const usize	n_adds		= M * K;
		const usize	flops		= n_muls + n_adds;
		const usize	data_size	= sizeof(float) * (M * K + M + K);	// RAM read bandwidth

		const auto	Print = [flops, data_size](secondsd dt)
		{{
			return	ToStringSfx( double(flops) / dt.count() ) << "FLOPS | " <<
					ToStringSfx( double(data_size) / dt.count() ) << "B/s";
		}};

		DynUntypedStorage	A;	A.Alloc( SizeOf<float> * K,		64_b, null );
		DynUntypedStorage	B;	B.Alloc( SizeOf<float> * M * K,	64_b, null );
		DynUntypedStorage	C;	C.Alloc( SizeOf<float> * M,		64_b, null );
		DynUntypedStorage	R;	R.Alloc( SizeOf<float> * M,		64_b, null );

		const auto	Reset = [&] ()
		{{
			FillWithLinearData<float>( A, 0.1f,  2.f );
			FillWithLinearData<float>( B, 0.4f, -1.f );
			FillWithLinearData<float>( C, 0.8f,  3.f );
		}};

		IntervalProfiler	profiler{ "GEMV MulAdd "s <<ToString(M)<<'x'<<ToString(K)<<" on "<<ToString(coreType)<<" core" };

	#if AE_SIMD_AVX >= 2
		{
			profiler.BeginTest( "AVX2 v1", Print );
			Reset();
			for (usize i = 0; i < max_iter; ++i)
			{
				MemCopy( OUT R.Data(), R.Size(), C.Data(), C.Size() );

				profiler.BeginIteration();
				avx_v1::MatVecMulAdd( A.Ptr<float>(), B.Ptr<float>(), R.Ptr<float>(), M, K );
				profiler.EndIteration();
			}
			profiler.EndTest();
			TestVecMatMulAdd<float>( A, B, C, R, K, MatrixDim{K,M}, M, col_maj, "AVX2 v1" );
		}{
			profiler.BeginTest( "AVX2 v2", Print );
			Reset();
			for (usize i = 0; i < max_iter; ++i)
			{
				MemCopy( OUT R.Data(), R.Size(), C.Data(), C.Size() );

				profiler.BeginIteration();
				avx_v2::MatVecMulAdd( A.Ptr<float>(), B.Ptr<float>(), R.Ptr<float>(), M, K );
				profiler.EndIteration();
			}
			profiler.EndTest();
			TestVecMatMulAdd<float>( A, B, C, R, K, MatrixDim{K,M}, M, col_maj, "AVX2 v2" );
		}{
			profiler.BeginTest( "AVX2 v3", Print );
			Reset();
			for (usize i = 0; i < max_iter; ++i)
			{
				MemCopy( OUT R.Data(), R.Size(), C.Data(), C.Size() );

				profiler.BeginIteration();
				avx_v3::MatVecMulAdd( A.Ptr<float>(), B.Ptr<float>(), R.Ptr<float>(), M, K );
				profiler.EndIteration();
			}
			profiler.EndTest();
			TestVecMatMulAdd<float>( A, B, C, R, K, MatrixDim{K,M}, M, col_maj, "AVX2 v3" );
		}{
			profiler.BeginTest( "AVX2 v4", Print );
			Reset();
			for (usize i = 0; i < max_iter; ++i)
			{
				MemCopy( OUT R.Data(), R.Size(), C.Data(), C.Size() );

				profiler.BeginIteration();
				avx_v4::MatVecMulAdd( A.Ptr<float>(), B.Ptr<float>(), R.Ptr<float>(), M, K );
				profiler.EndIteration();
			}
			profiler.EndTest();
			TestVecMatMulAdd<float>( A, B, C, R, K, MatrixDim{K,M}, M, col_maj, "AVX2 v4" );
		}
	#endif // AVX2
	#if AE_SIMD_NEON
		{
			profiler.BeginTest( "NEON v1", Print );
			Reset();
			for (usize i = 0; i < max_iter; ++i)
			{
				MemCopy( OUT R.Data(), R.Size(), C.Data(), C.Size() );

				profiler.BeginIteration();
				neon_v1::MatVecMulAdd( A.Ptr<float>(), B.Ptr<float>(), R.Ptr<float>(), M, K );
				profiler.EndIteration();
			}
			profiler.EndTest();
			TestVecMatMulAdd<float>( A, B, C, R, K, MatrixDim{K,M}, M, col_maj, "NEON v1" );
		}{
			profiler.BeginTest( "NEON v2", Print );
			Reset();
			for (usize i = 0; i < max_iter; ++i)
			{
				MemCopy( OUT R.Data(), R.Size(), C.Data(), C.Size() );

				profiler.BeginIteration();
				neon_v2::MatVecMulAdd( A.Ptr<float>(), B.Ptr<float>(), R.Ptr<float>(), M, K );
				profiler.EndIteration();
			}
			profiler.EndTest();
			TestVecMatMulAdd<float>( A, B, C, R, K, MatrixDim{K,M}, M, col_maj, "NEON v2" );
		}{
			profiler.BeginTest( "NEON v3", Print );
			Reset();
			for (usize i = 0; i < max_iter; ++i)
			{
				MemCopy( OUT R.Data(), R.Size(), C.Data(), C.Size() );

				profiler.BeginIteration();
				neon_v3::MatVecMulAdd( A.Ptr<float>(), B.Ptr<float>(), R.Ptr<float>(), M, K );
				profiler.EndIteration();
			}
			profiler.EndTest();
			TestVecMatMulAdd<float>( A, B, C, R, K, MatrixDim{K,M}, M, col_maj, "NEON v3" );
		}{
			profiler.BeginTest( "NEON v4", Print );
			Reset();
			for (usize i = 0; i < max_iter; ++i)
			{
				MemCopy( OUT R.Data(), R.Size(), C.Data(), C.Size() );

				profiler.BeginIteration();
				neon_v4::MatVecMulAdd( A.Ptr<float>(), B.Ptr<float>(), R.Ptr<float>(), M, K );
				profiler.EndIteration();
			}
			profiler.EndTest();
			TestVecMatMulAdd<float>( A, B, C, R, K, MatrixDim{K,M}, M, col_maj, "NEON v4" );
		}
	#endif // NEON
		{
			profiler.BeginTest( "AE GEMV", Print );
			Reset();
			for (usize i = 0; i < max_iter; ++i)
			{
				MemCopy( OUT R.Data(), R.Size(), C.Data(), C.Size() );

				profiler.BeginIteration();
				GEMV::MulAdd(	LargeVectorView<const float>						 { A.Data<float>(), A.Size(), K },
								LargeMatrixView<const float, EMatrixLayout::RowMajor>{ B.Data<float>(), B.Size(), M, K },
								INOUT LargeVectorView<float>						 { R.Data<float>(), R.Size(), M });
				profiler.EndIteration();
			}
			profiler.EndTest();
			TestVecMatMulAdd<float>( A, B, C, R, K, MatrixDim{K,M}, M, col_maj, "AE GEMV" );
		}
	}


	static void  GEMV_Test1 (ECoreType coreType)
	{
		const uint	M			= 2048;
		const uint	K			= 2048;
		#ifdef AE_DEBUG
		const usize	max_iter	= 1;
		#else
		const usize	max_iter	= coreType > ECoreType::EnergyEfficient ? 10'000 : 100;
		#endif

		GEMVTest( M, K, max_iter, coreType );
	}

	static void  GEMV_Test2 (ECoreType coreType)
	{
		const uint	M			= 256;
		const uint	K			= 256;
		#ifdef AE_DEBUG
		const usize	max_iter	= 1;
		#else
		const usize	max_iter	= coreType > ECoreType::EnergyEfficient ? 100'000'000 : 100'000;
		#endif

		GEMVTest( M, K, max_iter, coreType );
	}

} // namespace


extern void PerfTest_SimdGEMV ()
{
#if 0
	GEMV_Test2( ECoreType::Performance );
#else
	ForEachCoreType(
		[&] (auto &core, Function<void()> setAffinity)
		{
			setAffinity();

			GEMV_Test1( core.type );
			GEMV_Test2( core.type );
		});
#endif

	TEST_PASSED();
}
