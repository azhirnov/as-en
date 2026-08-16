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

	static void  TestMatMulAdd (ArrayView<float> inputA, ArrayView<float> inputB, ArrayView<float> inputC, ArrayView<float> refOutput,
								const MatrixDim dimA, const MatrixDim dimB, const MatrixDim dimC, const bool columnMajor, StringView testName)
	{
		ASSERT( inputA.size() == dimA.rows * dimA.columns );
		ASSERT( inputB.size() == dimB.rows * dimB.columns );
		ASSERT( inputC.size() == dimC.rows * dimC.columns );

		ASSERT( dimA.columns == dimB.rows );
		ASSERT( dimA.rows	 == dimC.rows );
		ASSERT( dimB.columns == dimC.columns );

		Array<float>	output;
		output.resize( dimC.Size() );

		if ( columnMajor )
		{
			for (uint i = 0; i < dimA.rows; ++i)
			{
				for (uint j = 0; j < dimB.columns; ++j)
				{
					const uint	idx  = j * dimC.rows + i;
					float		sum  = float(inputC[ idx ]);

					for (uint k = 0; k < dimA.columns; ++k)
					{
						float	a = float(inputA[ k * dimA.rows + i ]);		// col
						float	b = float(inputB[ j * dimB.rows + k ]);		// row

						sum += a * b;
					}

					output[ idx ] = sum;
				}
			}
		}
		else // row-major
		{
			for (uint i = 0; i < dimA.rows; ++i)
			{
				for (uint j = 0; j < dimB.columns; ++j)
				{
					const uint	idx  = i * dimC.columns + j;
					float		sum  = float(inputC[ idx ]);

					for (uint k = 0; k < dimA.columns; ++k)
					{
						float	a = float(inputA[ i * dimA.columns + k ]);		// row
						float	b = float(inputB[ k * dimB.columns + j ]);		// col

						sum += a * b;
					}

					output[ idx ] = sum;
				}
			}
		}

		String	str;
		float	max_err	= 0.f;
		float	avr_err	= 0.f;

		//str << "\n| C,R | expected | SIMD output | error % |\n";

		for (uint i = 0; i < dimC.rows; ++i)
		for (uint j = 0; j < dimC.columns; ++j)
		{
			uint	idx = columnMajor ?
							j * dimC.rows + i :
							i * dimC.columns + j;

			float	err	= Abs(output[idx] - refOutput[idx]) * 100.f / Max( Abs(output[idx]), 1.0e-5f );
			max_err = Max( max_err, err );
			avr_err += err;

		//	str << "| " << ToString( i ) << ", " << ToString( j ) << " | "
		//		<< ToString( output[idx], 5 ) << " | " << ToString( refOutput[idx], 5 ) << " | "
		//		<< ToString( err, 2 ) << "% |\n";
		}
		avr_err /= float(dimC.Size());

		str << testName << ": max error: " << ToString( max_err, 2 ) << "%, avr: " << ToString( avr_err, 2 ) << '%';
		//str << "\n----------------------------------------\n\n";
		AE_LOGI( str );
		CHECK( max_err < 1.f );
	}

	template <typename T>
	static void  TestMatMulAdd (const DynUntypedStorage &inputA, const DynUntypedStorage &inputB,
								const DynUntypedStorage &inputC, const DynUntypedStorage &refOutput,
								const MatrixDim colRowA, const MatrixDim colRowB, const MatrixDim colRowC, const bool columnMajor,
								StringView testName)
	{
		TestMatMulAdd( ArrayView<T>{ inputA.Ptr<T>(), usize{inputA.Size() / SizeOf<T>} },
					   ArrayView<T>{ inputB.Ptr<T>(), usize{inputB.Size() / SizeOf<T>} },
					   ArrayView<T>{ inputC.Ptr<T>(), usize{inputC.Size() / SizeOf<T>} },
					   ArrayView<T>{ refOutput.Ptr<T>(), usize{refOutput.Size() / SizeOf<T>} },
					   colRowA, colRowB, colRowC, columnMajor, testName );
	}
//-----------------------------------------------------------------------------


#if AE_SIMD_AVX >= 2

namespace avx2_v1
{
	// Row-major matrices:
	// A[M][K], B[K][N], C[M][N]
	//
	// Computes: C += A * B
	void MatrixMulAdd(
		const float* RESTRICT A,
		const float* RESTRICT B,
		float* RESTRICT C,
		std::size_t M,
		std::size_t N,
		std::size_t K)
	{
		// Tune these values for the target CPU/cache hierarchy.
		constexpr std::size_t BLOCK_M = 128;
		constexpr std::size_t BLOCK_N = 128;
		constexpr std::size_t BLOCK_K = 256;

		//const auto working_set = sizeof(float) * (BLOCK_M * BLOCK_K + BLOCK_K * BLOCK_N + BLOCK_M * BLOCK_N);
		//AE_LOGI( "Size "s << ToString(Bytes{working_set}) << " must be 80% of L2" );

		for (std::size_t jj = 0; jj < N; jj += BLOCK_N)
		{
			const std::size_t jEnd = std::min(jj + BLOCK_N, N);

			for (std::size_t kk = 0; kk < K; kk += BLOCK_K)
			{
				const std::size_t kEnd = std::min(kk + BLOCK_K, K);

				for (std::size_t ii = 0; ii < M; ii += BLOCK_M)
				{
					const std::size_t iEnd = std::min(ii + BLOCK_M, M);

					std::size_t i = ii;

					// 4 rows x 8 columns micro-kernel.
					// latency limited!!!
					for (; i + 4 <= iEnd; i += 4)
					{
						std::size_t j = jj;

						for (; j + 8 <= jEnd; j += 8)
						{
							__m256 c0 = _mm256_loadu_ps(C + (i + 0) * N + j);
							__m256 c1 = _mm256_loadu_ps(C + (i + 1) * N + j);
							__m256 c2 = _mm256_loadu_ps(C + (i + 2) * N + j);
							__m256 c3 = _mm256_loadu_ps(C + (i + 3) * N + j);

							for (std::size_t k = kk; k < kEnd; ++k)
							{
								const __m256 b = _mm256_loadu_ps(B + k * N + j);

								const __m256 a0 = _mm256_broadcast_ss(A + (i + 0) * K + k);
								const __m256 a1 = _mm256_broadcast_ss(A + (i + 1) * K + k);
								const __m256 a2 = _mm256_broadcast_ss(A + (i + 2) * K + k);
								const __m256 a3 = _mm256_broadcast_ss(A + (i + 3) * K + k);

								c0 = _mm256_fmadd_ps(a0, b, c0);
								c1 = _mm256_fmadd_ps(a1, b, c1);
								c2 = _mm256_fmadd_ps(a2, b, c2);
								c3 = _mm256_fmadd_ps(a3, b, c3);
							}

							_mm256_storeu_ps(C + (i + 0) * N + j, c0);
							_mm256_storeu_ps(C + (i + 1) * N + j, c1);
							_mm256_storeu_ps(C + (i + 2) * N + j, c2);
							_mm256_storeu_ps(C + (i + 3) * N + j, c3);
						}

						// Remaining columns.
						for (; j < jEnd; ++j)
						{
							float c0 = C[(i + 0) * N + j];
							float c1 = C[(i + 1) * N + j];
							float c2 = C[(i + 2) * N + j];
							float c3 = C[(i + 3) * N + j];

							for (std::size_t k = kk; k < kEnd; ++k)
							{
								const float b = B[k * N + j];

								c0 += A[(i + 0) * K + k] * b;
								c1 += A[(i + 1) * K + k] * b;
								c2 += A[(i + 2) * K + k] * b;
								c3 += A[(i + 3) * K + k] * b;
							}

							C[(i + 0) * N + j] = c0;
							C[(i + 1) * N + j] = c1;
							C[(i + 2) * N + j] = c2;
							C[(i + 3) * N + j] = c3;
						}
					}

					// Remaining rows.
					for (; i < iEnd; ++i)
					{
						std::size_t j = jj;

						for (; j + 8 <= jEnd; j += 8)
						{
							__m256 c = _mm256_loadu_ps(C + i * N + j);

							for (std::size_t k = kk; k < kEnd; ++k)
							{
								const __m256 a = _mm256_broadcast_ss(A + i * K + k);
								const __m256 b = _mm256_loadu_ps(B + k * N + j);

								c = _mm256_fmadd_ps(a, b, c);
							}

							_mm256_storeu_ps(C + i * N + j, c);
						}

						for (; j < jEnd; ++j)
						{
							float value = C[i * N + j];

							for (std::size_t k = kk; k < kEnd; ++k)
							{
								value += A[i * K + k] * B[k * N + j];
							}

							C[i * N + j] = value;
						}
					}
				}
			}
		}
	}
} // avx2_v1
//-----------------------------------------------------------------------------


namespace avx2_v2
{
	// Standard 6x16 Microkernel for AVX2 FMA (Processing 6 rows of A and 2 vector columns of B)
	void avx2_gemm_microkernel_6x16(
		const float* RESTRICT A, int lda, // Matrix A ptr & leading dimension
		const float* RESTRICT B, int ldb, // Matrix B ptr & leading dimension
		float* RESTRICT C,       int ldc, // Matrix C (Output/Accumulator) ptr
		int K)                   // Inner dimension depth
	{
		// Initialize 12 YMM accumulator registers to hold the 6x16 output block
		__m256 c00 = _mm256_loadu_ps(C + 0 * ldc + 0);
		__m256 c01 = _mm256_loadu_ps(C + 0 * ldc + 8);
		__m256 c10 = _mm256_loadu_ps(C + 1 * ldc + 0);
		__m256 c11 = _mm256_loadu_ps(C + 1 * ldc + 8);
		__m256 c20 = _mm256_loadu_ps(C + 2 * ldc + 0);
		__m256 c21 = _mm256_loadu_ps(C + 2 * ldc + 8);
		__m256 c30 = _mm256_loadu_ps(C + 3 * ldc + 0);
		__m256 c31 = _mm256_loadu_ps(C + 3 * ldc + 8);
		__m256 c40 = _mm256_loadu_ps(C + 4 * ldc + 0);
		__m256 c41 = _mm256_loadu_ps(C + 4 * ldc + 8);
		__m256 c50 = _mm256_loadu_ps(C + 5 * ldc + 0);
		__m256 c51 = _mm256_loadu_ps(C + 5 * ldc + 8);

		for (int k = 0; k < K; ++k)
		{
			// Load two sequential AVX2 registers from B (16 elements total)
			__m256 b0 = _mm256_loadu_ps(B + k * ldb + 0);
			__m256 b1 = _mm256_loadu_ps(B + k * ldb + 8);

			// Broadcast values from individual rows of A and multiply-add
			__m256 a0 = _mm256_set1_ps(A[0 * lda + k]);
			c00 = _mm256_fmadd_ps(a0, b0, c00);
			c01 = _mm256_fmadd_ps(a0, b1, c01);

			__m256 a1 = _mm256_set1_ps(A[1 * lda + k]);
			c10 = _mm256_fmadd_ps(a1, b0, c10);
			c11 = _mm256_fmadd_ps(a1, b1, c11);

			__m256 a2 = _mm256_set1_ps(A[2 * lda + k]);
			c20 = _mm256_fmadd_ps(a2, b0, c20);
			c21 = _mm256_fmadd_ps(a2, b1, c21);

			__m256 a3 = _mm256_set1_ps(A[3 * lda + k]);
			c30 = _mm256_fmadd_ps(a3, b0, c30);
			c31 = _mm256_fmadd_ps(a3, b1, c31);

			__m256 a4 = _mm256_set1_ps(A[4 * lda + k]);
			c40 = _mm256_fmadd_ps(a4, b0, c40);
			c41 = _mm256_fmadd_ps(a4, b1, c41);

			__m256 a5 = _mm256_set1_ps(A[5 * lda + k]);
			c50 = _mm256_fmadd_ps(a5, b0, c50);
			c51 = _mm256_fmadd_ps(a5, b1, c51);
		}

		_mm256_storeu_ps(C + 0 * ldc + 0, c00);
		_mm256_storeu_ps(C + 0 * ldc + 8, c01);
		_mm256_storeu_ps(C + 1 * ldc + 0, c10);
		_mm256_storeu_ps(C + 1 * ldc + 8, c11);
		_mm256_storeu_ps(C + 2 * ldc + 0, c20);
		_mm256_storeu_ps(C + 2 * ldc + 8, c21);
		_mm256_storeu_ps(C + 3 * ldc + 0, c30);
		_mm256_storeu_ps(C + 3 * ldc + 8, c31);
		_mm256_storeu_ps(C + 4 * ldc + 0, c40);
		_mm256_storeu_ps(C + 4 * ldc + 8, c41);
		_mm256_storeu_ps(C + 5 * ldc + 0, c50);
		_mm256_storeu_ps(C + 5 * ldc + 8, c51);
	}


	void MatrixMulAdd(const float* RESTRICT A,
					  const float* RESTRICT B,
					  float* RESTRICT C,
					  const int M, const int N, const int K)
	{
		const int lda = K;
		const int ldb = N;
		const int ldc = N;

		const int M_simd_bound = M - (M % 6);

		for (int i = 0; i < M_simd_bound; i += 6)
		{
			for (int j = 0; j < N; j += 16)
			{
				const float* A_tile = A + (i * lda);
				const float* B_tile = B + j;
				float*       C_tile = C + (i * ldc) + j;

				avx2_gemm_microkernel_6x16(A_tile, lda, B_tile, ldb, C_tile, ldc, K);
			}
		}

		for (int i = M_simd_bound; i < M; ++i)
		{
			for (int k = 0; k < K; ++k)
			{
				float a_val = A[i * lda + k];

				for (int j = 0; j < N; ++j)
				{
					C[i * ldc + j] += a_val * B[k * ldb + j];
				}
			}
		}
	}

} // avx2_v2
//-----------------------------------------------------------------------------


namespace avx2_v3 {

	// Register microkernel dimensions.
	constexpr std::size_t MR = 6;
	constexpr std::size_t NR = 16;

	// Starting values; tune for the target CPU.
	//
	// Packed A panel: approximately MC * KC * sizeof(float)
	// Packed B panel: approximately KC * NC * sizeof(float)
	constexpr std::size_t MC = 120; // Must preferably be a multiple of MR.
	constexpr std::size_t NC = 256; // Must preferably be a multiple of NR.
	constexpr std::size_t KC = 256;

	constexpr std::size_t round_up(std::size_t x, std::size_t multiple)
	{
		return (x + multiple - 1) / multiple * multiple;
	}

	/*
	 * Packed A layout, one 6-row micro-panel:
	 *
	 *   k=0: A[row+0][k], ..., A[row+5][k]
	 *   k=1: A[row+0][k], ..., A[row+5][k]
	 *   ...
	 *
	 * Each micro-panel occupies kc * MR floats.
	 */
	static void pack_a_panel(
		const float* RESTRICT A,
		float* RESTRICT packed_a,
		std::size_t lda,
		std::size_t row_begin,
		std::size_t k_begin,
		std::size_t mc,
		std::size_t kc)
	{
		const std::size_t row_panels = (mc + MR - 1) / MR;

		for (std::size_t panel = 0; panel < row_panels; ++panel) {
			const std::size_t local_row = panel * MR;
			float* dst_panel = packed_a + panel * kc * MR;

			const std::size_t valid_rows =
				std::min(MR, mc - local_row);

			for (std::size_t k = 0; k < kc; ++k) {
				float* dst = dst_panel + k * MR;

				const float* src =
					A + (row_begin + local_row) * lda + (k_begin + k);

				std::size_t r = 0;

				for (; r < valid_rows; ++r)
					dst[r] = src[r * lda];

				// Zero-pad the final row micro-panel.
				for (; r < MR; ++r)
					dst[r] = 0.0f;
			}
		}
	}

	/*
	 * Packed B layout, one 16-column micro-panel:
	 *
	 *   k=0: B[k][column+0], ..., B[k][column+15]
	 *   k=1: B[k][column+0], ..., B[k][column+15]
	 *   ...
	 *
	 * Each micro-panel occupies kc * NR floats.
	 */
	static void pack_b_panel(
		const float* RESTRICT B,
		float* RESTRICT packed_b,
		std::size_t ldb,
		std::size_t k_begin,
		std::size_t column_begin,
		std::size_t kc,
		std::size_t nc)
	{
		const std::size_t column_panels = (nc + NR - 1) / NR;

		for (std::size_t panel = 0; panel < column_panels; ++panel) {
			const std::size_t local_column = panel * NR;
			float* dst_panel = packed_b + panel * kc * NR;

			const std::size_t valid_columns =
				std::min(NR, nc - local_column);

			for (std::size_t k = 0; k < kc; ++k) {
				float* dst = dst_panel + k * NR;

				const float* src =
					B + (k_begin + k) * ldb
					  + column_begin + local_column;

				std::size_t j = 0;

				for (; j < valid_columns; ++j)
					dst[j] = src[j];

				// Zero-pad the final column micro-panel.
				for (; j < NR; ++j)
					dst[j] = 0.0f;
			}
		}
	}

	/*
	 * Compute one full 6x16 output tile:
	 *
	 *     C[6x16] += packed_A[6xK] * packed_B[Kx16]
	 *
	 * packed_A advances by 6 floats per k.
	 * packed_B advances by 16 floats per k.
	 */
	static void microkernel_6x16(
		const float* RESTRICT packed_a,
		const float* RESTRICT packed_b,
		float* RESTRICT C,
		std::size_t ldc,
		std::size_t kc)
	{
		__m256 c00 = _mm256_loadu_ps(C + 0 * ldc + 0);
		__m256 c01 = _mm256_loadu_ps(C + 0 * ldc + 8);

		__m256 c10 = _mm256_loadu_ps(C + 1 * ldc + 0);
		__m256 c11 = _mm256_loadu_ps(C + 1 * ldc + 8);

		__m256 c20 = _mm256_loadu_ps(C + 2 * ldc + 0);
		__m256 c21 = _mm256_loadu_ps(C + 2 * ldc + 8);

		__m256 c30 = _mm256_loadu_ps(C + 3 * ldc + 0);
		__m256 c31 = _mm256_loadu_ps(C + 3 * ldc + 8);

		__m256 c40 = _mm256_loadu_ps(C + 4 * ldc + 0);
		__m256 c41 = _mm256_loadu_ps(C + 4 * ldc + 8);

		__m256 c50 = _mm256_loadu_ps(C + 5 * ldc + 0);
		__m256 c51 = _mm256_loadu_ps(C + 5 * ldc + 8);

		for (std::size_t k = 0; k < kc; ++k) {
			const __m256 b0 = _mm256_loadu_ps(packed_b + 0);
			const __m256 b1 = _mm256_loadu_ps(packed_b + 8);

			// Reuse one broadcast register to avoid excessive register pressure.
			__m256 a;

			a = _mm256_broadcast_ss(packed_a + 0);
			c00 = _mm256_fmadd_ps(a, b0, c00);
			c01 = _mm256_fmadd_ps(a, b1, c01);

			a = _mm256_broadcast_ss(packed_a + 1);
			c10 = _mm256_fmadd_ps(a, b0, c10);
			c11 = _mm256_fmadd_ps(a, b1, c11);

			a = _mm256_broadcast_ss(packed_a + 2);
			c20 = _mm256_fmadd_ps(a, b0, c20);
			c21 = _mm256_fmadd_ps(a, b1, c21);

			a = _mm256_broadcast_ss(packed_a + 3);
			c30 = _mm256_fmadd_ps(a, b0, c30);
			c31 = _mm256_fmadd_ps(a, b1, c31);

			a = _mm256_broadcast_ss(packed_a + 4);
			c40 = _mm256_fmadd_ps(a, b0, c40);
			c41 = _mm256_fmadd_ps(a, b1, c41);

			a = _mm256_broadcast_ss(packed_a + 5);
			c50 = _mm256_fmadd_ps(a, b0, c50);
			c51 = _mm256_fmadd_ps(a, b1, c51);

			packed_a += MR;
			packed_b += NR;
		}

		_mm256_storeu_ps(C + 0 * ldc + 0, c00);
		_mm256_storeu_ps(C + 0 * ldc + 8, c01);

		_mm256_storeu_ps(C + 1 * ldc + 0, c10);
		_mm256_storeu_ps(C + 1 * ldc + 8, c11);

		_mm256_storeu_ps(C + 2 * ldc + 0, c20);
		_mm256_storeu_ps(C + 2 * ldc + 8, c21);

		_mm256_storeu_ps(C + 3 * ldc + 0, c30);
		_mm256_storeu_ps(C + 3 * ldc + 8, c31);

		_mm256_storeu_ps(C + 4 * ldc + 0, c40);
		_mm256_storeu_ps(C + 4 * ldc + 8, c41);

		_mm256_storeu_ps(C + 5 * ldc + 0, c50);
		_mm256_storeu_ps(C + 5 * ldc + 8, c51);
	}

	/*
	 * Edge kernel for tiles smaller than 6x16.
	 *
	 * A temporary 6x16 tile allows the main kernel to remain branch-free.
	 * Invalid packed A/B values have already been zero-padded.
	 */
	static void microkernel_edge(
		const float* RESTRICT packed_a,
		const float* RESTRICT packed_b,
		float* RESTRICT C,
		std::size_t ldc,
		std::size_t kc,
		std::size_t valid_rows,
		std::size_t valid_columns)
	{
		alignas(32) float temporary[MR * NR] = {};

		// Copy the valid part of C into the temporary full-sized tile.
		for (std::size_t r = 0; r < valid_rows; ++r) {
			for (std::size_t j = 0; j < valid_columns; ++j)
				temporary[r * NR + j] = C[r * ldc + j];
		}

		microkernel_6x16(
			packed_a,
			packed_b,
			temporary,
			NR,
			kc);

		// Copy only the valid output region back.
		for (std::size_t r = 0; r < valid_rows; ++r) {
			for (std::size_t j = 0; j < valid_columns; ++j)
				C[r * ldc + j] = temporary[r * NR + j];
		}
	}

	/*
	 * Row-major matrix multiplication-add:
	 *
	 *     C[M][N] += A[M][K] * B[K][N]
	 *
	 * This function allocates reusable packed-panel buffers once per call.
	 */
	void matrix_mul_add_avx2(
		const float* RESTRICT A,
		const float* RESTRICT B,
		float* RESTRICT C,
		std::size_t M,
		std::size_t N,
		std::size_t K)
	{
		if (M == 0 || N == 0 || K == 0)
			return;

		constexpr std::size_t max_packed_a_rows = round_up(MC, MR);
		constexpr std::size_t max_packed_b_cols = round_up(NC, NR);

		std::vector<float> packed_a(max_packed_a_rows * KC);
		std::vector<float> packed_b(KC * max_packed_b_cols);

		/*
		 * jc: column panel of B/C
		 * pc: depth panel of A/B
		 * ic: row panel of A/C
		 */
		for (std::size_t jc = 0; jc < N; jc += NC)
		{
			const std::size_t nc = std::min(NC, N - jc);
			const std::size_t column_panels = (nc + NR - 1) / NR;

			for (std::size_t pc = 0; pc < K; pc += KC)
			{
				const std::size_t kc = std::min(KC, K - pc);

				// Reused by every MC row panel under this jc/pc panel.
				pack_b_panel(B, packed_b.data(), N, pc, jc, kc, nc);

				for (std::size_t ic = 0; ic < M; ic += MC)
				{
					const std::size_t mc = std::min(MC, M - ic);
					const std::size_t row_panels = (mc + MR - 1) / MR;

					// Reused by every 16-column micro-panel.
					pack_a_panel(A, packed_a.data(), K, ic, pc, mc, kc);

					for (std::size_t jp = 0; jp < column_panels; ++jp)
					{
						const std::size_t local_column = jp * NR;
						const std::size_t valid_columns = std::min(NR, nc - local_column);

						const float* b_micro_panel = packed_b.data() + jp * kc * NR;

						for (std::size_t ip = 0; ip < row_panels; ++ip)
						{
							const std::size_t local_row = ip * MR;
							const std::size_t valid_rows = std::min(MR, mc - local_row);

							const float* a_micro_panel = packed_a.data() + ip * kc * MR;

							float* c_tile = C + (ic + local_row) * N + jc + local_column;

							if (valid_rows == MR && valid_columns == NR)
							{
								microkernel_6x16(a_micro_panel, b_micro_panel, c_tile, N, kc);
							} else
							{
								microkernel_edge(a_micro_panel, b_micro_panel, c_tile, N, kc, valid_rows, valid_columns);
							}
						}
					}
				}
			}
		}
	}
} // avx2_v3
//-----------------------------------------------------------------------------


namespace avx2_v4
{
  #if AE_SIMD_FMA
	#define AE_FMADD_PS( a, b, c )  _mm256_fmadd_ps( (a), (b), (c) )
  #else
	#define AE_FMADD_PS( a, b, c )  _mm256_add_ps( _mm256_mul_ps( (a), (b) ), (c) )
  #endif

	static void  Microkernel_8x8 (const float*	RESTRICT	A, const uint lda,
								  const float*	RESTRICT	B, const uint ldb,
								  INOUT float* RESTRICT	C, const uint ldc,
								  const uint				K) __NE___
	{
		__m256	c0 = _mm256_loadu_ps( C + 0 * ldc );
		__m256	c1 = _mm256_loadu_ps( C + 1 * ldc );
		__m256	c2 = _mm256_loadu_ps( C + 2 * ldc );
		__m256	c3 = _mm256_loadu_ps( C + 3 * ldc );
		__m256	c4 = _mm256_loadu_ps( C + 4 * ldc );
		__m256	c5 = _mm256_loadu_ps( C + 5 * ldc );
		__m256	c6 = _mm256_loadu_ps( C + 6 * ldc );
		__m256	c7 = _mm256_loadu_ps( C + 7 * ldc );

		uint k = 0;

		// two rank-1 updates per iteration
		for (; k + 2 <= K; k += 2)
		{
			const __m256	b0 = _mm256_loadu_ps( B + (k + 0) * ldb );
			const __m256	b1 = _mm256_loadu_ps( B + (k + 1) * ldb );

			const float*	a0 = A + 0 * lda + k;
			const float*	a1 = A + 1 * lda + k;
			const float*	a2 = A + 2 * lda + k;
			const float*	a3 = A + 3 * lda + k;
			const float*	a4 = A + 4 * lda + k;
			const float*	a5 = A + 5 * lda + k;
			const float*	a6 = A + 6 * lda + k;
			const float*	a7 = A + 7 * lda + k;

			c0 = AE_FMADD_PS( _mm256_broadcast_ss( a0 ), b0, c0 );
			c1 = AE_FMADD_PS( _mm256_broadcast_ss( a1 ), b0, c1 );
			c2 = AE_FMADD_PS( _mm256_broadcast_ss( a2 ), b0, c2 );
			c3 = AE_FMADD_PS( _mm256_broadcast_ss( a3 ), b0, c3 );
			c4 = AE_FMADD_PS( _mm256_broadcast_ss( a4 ), b0, c4 );
			c5 = AE_FMADD_PS( _mm256_broadcast_ss( a5 ), b0, c5 );
			c6 = AE_FMADD_PS( _mm256_broadcast_ss( a6 ), b0, c6 );
			c7 = AE_FMADD_PS( _mm256_broadcast_ss( a7 ), b0, c7 );

			c0 = AE_FMADD_PS( _mm256_broadcast_ss( a0 + 1 ), b1, c0 );
			c1 = AE_FMADD_PS( _mm256_broadcast_ss( a1 + 1 ), b1, c1 );
			c2 = AE_FMADD_PS( _mm256_broadcast_ss( a2 + 1 ), b1, c2 );
			c3 = AE_FMADD_PS( _mm256_broadcast_ss( a3 + 1 ), b1, c3 );
			c4 = AE_FMADD_PS( _mm256_broadcast_ss( a4 + 1 ), b1, c4 );
			c5 = AE_FMADD_PS( _mm256_broadcast_ss( a5 + 1 ), b1, c5 );
			c6 = AE_FMADD_PS( _mm256_broadcast_ss( a6 + 1 ), b1, c6 );
			c7 = AE_FMADD_PS( _mm256_broadcast_ss( a7 + 1 ), b1, c7 );
		}

		// remainder when K is odd
		if ( k < K )
		{
			const __m256	b = _mm256_loadu_ps( B + k * ldb );

			c0 = AE_FMADD_PS( _mm256_broadcast_ss( A + 0 * lda + k ), b, c0 );
			c1 = AE_FMADD_PS( _mm256_broadcast_ss( A + 1 * lda + k ), b, c1 );
			c2 = AE_FMADD_PS( _mm256_broadcast_ss( A + 2 * lda + k ), b, c2 );
			c3 = AE_FMADD_PS( _mm256_broadcast_ss( A + 3 * lda + k ), b, c3 );
			c4 = AE_FMADD_PS( _mm256_broadcast_ss( A + 4 * lda + k ), b, c4 );
			c5 = AE_FMADD_PS( _mm256_broadcast_ss( A + 5 * lda + k ), b, c5 );
			c6 = AE_FMADD_PS( _mm256_broadcast_ss( A + 6 * lda + k ), b, c6 );
			c7 = AE_FMADD_PS( _mm256_broadcast_ss( A + 7 * lda + k ), b, c7 );
		}

		_mm256_storeu_ps( OUT C + 0 * ldc, c0 );
		_mm256_storeu_ps( OUT C + 1 * ldc, c1 );
		_mm256_storeu_ps( OUT C + 2 * ldc, c2 );
		_mm256_storeu_ps( OUT C + 3 * ldc, c3 );
		_mm256_storeu_ps( OUT C + 4 * ldc, c4 );
		_mm256_storeu_ps( OUT C + 5 * ldc, c5 );
		_mm256_storeu_ps( OUT C + 6 * ldc, c6 );
		_mm256_storeu_ps( OUT C + 7 * ldc, c7 );
	}

	static void  Microkernel_1x8 (const float*	RESTRICT	A,
								  const float*	RESTRICT	B,
								  INOUT float*	RESTRICT	C,
								  const uint				ldb,
								  const uint				K) __NE___
	{
		__m256	c = _mm256_loadu_ps( C );

		for (uint k = 0; k < K; ++k)
		{
			const __m256	a = _mm256_broadcast_ss( A + k );
			const __m256	b = _mm256_loadu_ps( B + k * ldb );

		  #if AE_SIMD_FMA
			c = _mm256_fmadd_ps( a, b, c );
		  #else
			c = _mm256_add_ps( _mm256_mul_ps( a, b ), c );
		  #endif
		}

		_mm256_storeu_ps( OUT C, c );
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

	static void  Microkernel_1x8_Masked (const float* RESTRICT	A,
										 const float* RESTRICT	B,
										 INOUT float* RESTRICT	C,
										 const uint			ldb,
										 const uint			K,
										 const uint			count) __NE___
	{
		// count is always in [1, 7] here, so a single masked vector suffices
		const __m256i	mask = Mask8( count );
		__m256			c	 = _mm256_maskload_ps( C, mask );

		for (uint k = 0; k < K; ++k)
		{
			const __m256	a = _mm256_broadcast_ss( A + k );
			const __m256	b = _mm256_maskload_ps( B + k * ldb, mask );

		  #if AE_SIMD_FMA
			c = _mm256_fmadd_ps( a, b, c );
		  #else
			c = _mm256_add_ps( _mm256_mul_ps( a, b ), c );
		  #endif
		}

		_mm256_maskstore_ps( OUT C, mask, c );
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

		const uint	M8 = M - (M % 8);
		const uint	N8 = N - (N % 8);

		for (uint i = 0; i < M8; i += 8)
		{
			const float*	A_tile = A + (i * lda);

			// complete 8x8 tiles
			for (uint j = 0; j < N8; j += 8)
			{
				Microkernel_8x8( A_tile, lda,
								 B + j, ldb,
								 INOUT C + (i * ldc) + j, ldc,
								 K );
			}

			// column tail (1..7 columns)
			if_unlikely( N8 != N )
			{
				for (uint r = 0; r < 8; ++r)
				{
					Microkernel_1x8_Masked(
						A_tile + r * lda,
						B + N8,
						INOUT C + (i + r) * ldc + N8,
						ldb,
						K,
						N - N8
					);
				}
			}
		}

		// row tail
		for (uint i = M8; i < M; ++i)
		{
			const float*	A_row = A + i * lda;
			float*			C_row = C + i * ldc;

			for (uint j = 0; j < N8; j += 8)
			{
				Microkernel_1x8( A_row, B + j, INOUT C_row + j, ldb, K );
			}

			if_unlikely( N8 != N )
			{
				Microkernel_1x8_Masked( A_row, B + N8, INOUT C_row + N8, ldb, K, N - N8 );
			}
		}
	}

} // avx2_v4
//-----------------------------------------------------------------------------

namespace avx2_v5
{
	static void  Microkernel_4x16 (const float*	RESTRICT	A, const uint lda,
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

		for (uint k = 0; k < K; ++k)
		{
			const __m256	b0 = _mm256_loadu_ps( B + k * ldb + 0 );
			const __m256	b1 = _mm256_loadu_ps( B + k * ldb + 8 );

			const __m256	a0 = _mm256_broadcast_ss( A + 0 * lda + k );
			c00 = AE_FMADD_PS( a0, b0, c00 );
			c01 = AE_FMADD_PS( a0, b1, c01 );

			const __m256	a1 = _mm256_broadcast_ss( A + 1 * lda + k );
			c10 = AE_FMADD_PS( a1, b0, c10 );
			c11 = AE_FMADD_PS( a1, b1, c11 );

			const __m256	a2 = _mm256_broadcast_ss( A + 2 * lda + k );
			c20 = AE_FMADD_PS( a2, b0, c20 );
			c21 = AE_FMADD_PS( a2, b1, c21 );

			const __m256	a3 = _mm256_broadcast_ss( A + 3 * lda + k );
			c30 = AE_FMADD_PS( a3, b0, c30 );
			c31 = AE_FMADD_PS( a3, b1, c31 );
		}

		_mm256_storeu_ps( OUT C + 0 * ldc + 0, c00 );
		_mm256_storeu_ps( OUT C + 0 * ldc + 8, c01 );
		_mm256_storeu_ps( OUT C + 1 * ldc + 0, c10 );
		_mm256_storeu_ps( OUT C + 1 * ldc + 8, c11 );
		_mm256_storeu_ps( OUT C + 2 * ldc + 0, c20 );
		_mm256_storeu_ps( OUT C + 2 * ldc + 8, c21 );
		_mm256_storeu_ps( OUT C + 3 * ldc + 0, c30 );
		_mm256_storeu_ps( OUT C + 3 * ldc + 8, c31 );
	}

	static void  Microkernel_1x16 (const float*	RESTRICT	A,
								   const float*	RESTRICT	B,
								   INOUT float*	RESTRICT	C,
								   const uint				ldb,
								   const uint				K) __NE___
	{
		__m256	c0 = _mm256_loadu_ps( C );
		__m256	c1 = _mm256_loadu_ps( C + 8 );

		for (uint k = 0; k < K; ++k)
		{
			const __m256	a  = _mm256_broadcast_ss( A + k );
			const __m256	b0 = _mm256_loadu_ps( B + k * ldb );
			const __m256	b1 = _mm256_loadu_ps( B + k * ldb + 8 );

			c0 = AE_FMADD_PS( a, b0, c0 );
			c1 = AE_FMADD_PS( a, b1, c1 );
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
				const __m256	a  = _mm256_broadcast_ss( A + k );
				const __m256	b0 = _mm256_maskload_ps( B + k * ldb, mask0 );

				c0 = AE_FMADD_PS( a, b0, c0 );
			}

			_mm256_maskstore_ps( OUT C, mask0, c0 );
			return;
		}

		const __m256i	mask1	= Mask8( count1 );
		__m256			c1		= _mm256_maskload_ps( C + 8, mask1 );

		for (uint k = 0; k < K; ++k)
		{
			const __m256	a  = _mm256_broadcast_ss( A + k );
			const float*	bk = B + k * ldb;

			const __m256	b0 = _mm256_loadu_ps( bk ); // count > 8, so all 8 are valid
			const __m256	b1 = _mm256_maskload_ps( bk + 8, mask1 );

			c0 = AE_FMADD_PS( a, b0, c0 );
			c1 = AE_FMADD_PS( a, b1, c1 );
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

		const uint	M4  = M - (M % 4);
		const uint	N16 = N - (N % 16);

		for (uint i = 0; i < M4; i += 4)
		{
			const float*	A_tile = A + (i * lda);

			// complete 4x16 tiles
			for (uint j = 0; j < N16; j += 16)
			{
				Microkernel_4x16( A_tile, lda,
								  B + j, ldb,
								  INOUT C + (i * ldc) + j, ldc,
								  K );
			}

			// column tail
			if_unlikely( N16 != N )
			{
				for (uint r = 0; r < 4; ++r)
				{
					Microkernel_1x16_Masked(
						A_tile + r * lda,
						B + N16,
						INOUT C + (i + r) * ldc + N16,
						ldb,
						K,
						N - N16
					);
				}
			}
		}

		// row tail
		for (uint i = M4; i < M; ++i)
		{
			const float*	A_row = A + i * lda;
			float*			C_row = C + i * ldc;

			for (uint j = 0; j < N16; j += 16)
			{
				Microkernel_1x16( A_row, B + j, INOUT C_row + j, ldb, K );
			}

			if_unlikely( N16 != N )
			{
				Microkernel_1x16_Masked( A_row, B + N16, INOUT C_row + N16, ldb, K, N - N16 );
			}
		}
	}
} // avx2_v5
//-----------------------------------------------------------------------------
#endif // AVX2


#if AE_SIMD_NEON

namespace neon_v1 {

	//---------------------------------------------------------------------
	//  Blocking (AArch64 only)
	//---------------------------------------------------------------------
	#define GM_MR   8       // micro-kernel rows
	#define GM_NR   8       // micro-kernel cols
	#define GM_KC   128     // K-block:  micro-panel = KC*8*4 =   4 KB (L1)
	#define GM_NC   256     // B block:  packed B    = KC*NC*4 = 128 KB (L2)

	inline uint  MinU (const uint a, const uint b) { return a < b ? a : b; }

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
		float32x4_t c00 = vld1q_f32(pc + 0*ldc + 0), c01 = vld1q_f32(pc + 0*ldc + 4);
		float32x4_t c10 = vld1q_f32(pc + 1*ldc + 0), c11 = vld1q_f32(pc + 1*ldc + 4);
		float32x4_t c20 = vld1q_f32(pc + 2*ldc + 0), c21 = vld1q_f32(pc + 2*ldc + 4);
		float32x4_t c30 = vld1q_f32(pc + 3*ldc + 0), c31 = vld1q_f32(pc + 3*ldc + 4);
		float32x4_t c40 = vld1q_f32(pc + 4*ldc + 0), c41 = vld1q_f32(pc + 4*ldc + 4);
		float32x4_t c50 = vld1q_f32(pc + 5*ldc + 0), c51 = vld1q_f32(pc + 5*ldc + 4);
		float32x4_t c60 = vld1q_f32(pc + 6*ldc + 0), c61 = vld1q_f32(pc + 6*ldc + 4);
		float32x4_t c70 = vld1q_f32(pc + 7*ldc + 0), c71 = vld1q_f32(pc + 7*ldc + 4);

		for (uint k = 0; k < kc; ++k)
		{
			const float32x4_t a0 = vld1q_f32(pa + 0);   // rows 0..3 at depth k
			const float32x4_t a1 = vld1q_f32(pa + 4);   // rows 4..7 at depth k
			const float32x4_t b0 = vld1q_f32(pb + 0);   // cols 0..3 at depth k
			const float32x4_t b1 = vld1q_f32(pb + 4);   // cols 4..7 at depth k

			c00 = vfmaq_laneq_f32(c00, b0, a0, 0);
			c01 = vfmaq_laneq_f32(c01, b1, a0, 0);
			c10 = vfmaq_laneq_f32(c10, b0, a0, 1);
			c11 = vfmaq_laneq_f32(c11, b1, a0, 1);
			c20 = vfmaq_laneq_f32(c20, b0, a0, 2);
			c21 = vfmaq_laneq_f32(c21, b1, a0, 2);
			c30 = vfmaq_laneq_f32(c30, b0, a0, 3);
			c31 = vfmaq_laneq_f32(c31, b1, a0, 3);
			c40 = vfmaq_laneq_f32(c40, b0, a1, 0);
			c41 = vfmaq_laneq_f32(c41, b1, a1, 0);
			c50 = vfmaq_laneq_f32(c50, b0, a1, 1);
			c51 = vfmaq_laneq_f32(c51, b1, a1, 1);
			c60 = vfmaq_laneq_f32(c60, b0, a1, 2);
			c61 = vfmaq_laneq_f32(c61, b1, a1, 2);
			c70 = vfmaq_laneq_f32(c70, b0, a1, 3);
			c71 = vfmaq_laneq_f32(c71, b1, a1, 3);

			pa += 8;
			pb += 8;
		}

		vst1q_f32(pc + 0*ldc + 0, c00);   vst1q_f32(pc + 0*ldc + 4, c01);
		vst1q_f32(pc + 1*ldc + 0, c10);   vst1q_f32(pc + 1*ldc + 4, c11);
		vst1q_f32(pc + 2*ldc + 0, c20);   vst1q_f32(pc + 2*ldc + 4, c21);
		vst1q_f32(pc + 3*ldc + 0, c30);   vst1q_f32(pc + 3*ldc + 4, c31);
		vst1q_f32(pc + 4*ldc + 0, c40);   vst1q_f32(pc + 4*ldc + 4, c41);
		vst1q_f32(pc + 5*ldc + 0, c50);   vst1q_f32(pc + 5*ldc + 4, c51);
		vst1q_f32(pc + 6*ldc + 0, c60);   vst1q_f32(pc + 6*ldc + 4, c61);
		vst1q_f32(pc + 7*ldc + 0, c70);   vst1q_f32(pc + 7*ldc + 4, c71);
	}

	//---------------------------------------------------------------------
	inline void  Transpose4x4 (const float* s0, const float* s1,
							   const float* s2, const float* s3,
							   float* d, const uint dstride)
	{
		const float32x4_t   r0 = vld1q_f32(s0), r1 = vld1q_f32(s1);
		const float32x4_t   r2 = vld1q_f32(s2), r3 = vld1q_f32(s3);
		const float32x4x2_t t01 = vtrnq_f32(r0, r1);
		const float32x4x2_t t23 = vtrnq_f32(r2, r3);

		vst1q_f32(d + 0*dstride, vcombine_f32(vget_low_f32 (t01.val[0]), vget_low_f32 (t23.val[0])));
		vst1q_f32(d + 1*dstride, vcombine_f32(vget_low_f32 (t01.val[1]), vget_low_f32 (t23.val[1])));
		vst1q_f32(d + 2*dstride, vcombine_f32(vget_high_f32(t01.val[0]), vget_high_f32(t23.val[0])));
		vst1q_f32(d + 3*dstride, vcombine_f32(vget_high_f32(t01.val[1]), vget_high_f32(t23.val[1])));
	}

	// Pack an 8 x kc panel of A (row stride lda) -> k-major.  kc % 4 == 0.
	inline void  PackAPanel (const float* RESTRICT src, float* RESTRICT dst,
							 const uint kc, const uint lda)
	{
		for (uint k = 0; k < kc; k += 4)
		{
			Transpose4x4(src + 0*lda + k, src + 1*lda + k,
						 src + 2*lda + k, src + 3*lda + k,
						 dst + k*8 + 0, 8);
			Transpose4x4(src + 4*lda + k, src + 5*lda + k,
						 src + 6*lda + k, src + 7*lda + k,
						 dst + k*8 + 4, 8);
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
				vst1q_f32(d + 0, vld1q_f32(s + 0));
				vst1q_f32(d + 4, vld1q_f32(s + 4));
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
		if (!M || !N || !K)
			return;

		alignas(64) float packedB[GM_KC * GM_NC];   // 128 KB, stack
		alignas(64) float packedA[GM_KC * GM_MR];   //   4 KB, stack

		for (uint jc = 0; jc < N; jc += GM_NC)
		{
			const uint nc = MinU(GM_NC, N - jc);

			for (uint k0 = 0; k0 < K; k0 += GM_KC)
			{
				const uint kc = MinU(GM_KC, K - k0);

				PackB(B + k0*N + jc, packedB, kc, nc, N);       // once per K-block

				for (uint ir = 0; ir < M; ir += GM_MR)
				{
					PackAPanel(A + ir*K + k0, packedA, kc, K);  // 4 KB, L1-resident

					float* RESTRICT cPanel = C + ir*N + jc;

					for (uint jr = 0; jr < nc; jr += GM_NR)
						MicroKernel(packedA, packedB + jr*kc, cPanel + jr, kc, N);
				}
			}
		}
	}

} // neon_v1
//-----------------------------------------------------------------------------

/*
namespace neon_v2
{
	// ===========================================================================
	// 8x8 SGEMM microkernel:  C[8x8] += A[8xK] * B[Kx8]
	//
	// Register budget (32 x 128-bit NEON regs):
	//    16 x float32x4_t  accumulators (8 rows x 2 halves)
	//     2 x float32x4_t  B row (8 contiguous floats)
	//     2 x float32x4_t  A column (8 strided scalars packed 4-per-reg)
	//    = 20/32 registers -> no spills, ~12 regs free for unrolling
	// ===========================================================================
	static inline void neon_gemm_microkernel_8x8(
		const float* RESTRICT A, int lda,
		const float* RESTRICT B, int ldb,
		float* RESTRICT C,       int ldc,
		int K)
	{
		// ---- 16 accumulators -------------------------------------------------
		float32x4_t c00 = vld1q_f32(C + 0*ldc + 0);
		float32x4_t c01 = vld1q_f32(C + 0*ldc + 4);
		float32x4_t c10 = vld1q_f32(C + 1*ldc + 0);
		float32x4_t c11 = vld1q_f32(C + 1*ldc + 4);
		float32x4_t c20 = vld1q_f32(C + 2*ldc + 0);
		float32x4_t c21 = vld1q_f32(C + 2*ldc + 4);
		float32x4_t c30 = vld1q_f32(C + 3*ldc + 0);
		float32x4_t c31 = vld1q_f32(C + 3*ldc + 4);
		float32x4_t c40 = vld1q_f32(C + 4*ldc + 0);
		float32x4_t c41 = vld1q_f32(C + 4*ldc + 4);
		float32x4_t c50 = vld1q_f32(C + 5*ldc + 0);
		float32x4_t c51 = vld1q_f32(C + 5*ldc + 4);
		float32x4_t c60 = vld1q_f32(C + 6*ldc + 0);
		float32x4_t c61 = vld1q_f32(C + 6*ldc + 4);
		float32x4_t c70 = vld1q_f32(C + 7*ldc + 0);
		float32x4_t c71 = vld1q_f32(C + 7*ldc + 4);

		const float* a0 = A + 0*lda;  const float* a1 = A + 1*lda;
		const float* a2 = A + 2*lda;  const float* a3 = A + 3*lda;
		const float* a4 = A + 4*lda;  const float* a5 = A + 5*lda;
		const float* a6 = A + 6*lda;  const float* a7 = A + 7*lda;

		for (int k = 0; k < K; ++k)
		{
			// ---- B: 8 contiguous floats (2 regs) ------------------------------
			float32x4_t b0 = vld1q_f32(B + k*ldb + 0);
			float32x4_t b1 = vld1q_f32(B + k*ldb + 4);

			// ---- A: gather column k (stride lda) into 2 regs ------------------
			// ld1r splat + 3 single-lane loads; other lanes stay untouched
			float32x4_t a0123 = vdupq_n_f32(a0[k]);
			a0123 = vld1q_lane_f32(a1 + k, a0123, 1);
			a0123 = vld1q_lane_f32(a2 + k, a0123, 2);
			a0123 = vld1q_lane_f32(a3 + k, a0123, 3);

			float32x4_t a4567 = vdupq_n_f32(a4[k]);
			a4567 = vld1q_lane_f32(a5 + k, a4567, 1);
			a4567 = vld1q_lane_f32(a6 + k, a4567, 2);
			a4567 = vld1q_lane_f32(a7 + k, a4567, 3);

			// ---- rank-1 update: lane-broadcast FMA, zero broadcast regs -------
			c00 = vfmaq_lane_f32(c00, b0, a0123, 0);
			c01 = vfmaq_lane_f32(c01, b1, a0123, 0);
			c10 = vfmaq_lane_f32(c10, b0, a0123, 1);
			c11 = vfmaq_lane_f32(c11, b1, a0123, 1);
			c20 = vfmaq_lane_f32(c20, b0, a0123, 2);
			c21 = vfmaq_lane_f32(c21, b1, a0123, 2);
			c30 = vfmaq_lane_f32(c30, b0, a0123, 3);
			c31 = vfmaq_lane_f32(c31, b1, a0123, 3);

			c40 = vfmaq_lane_f32(c40, b0, a4567, 0);
			c41 = vfmaq_lane_f32(c41, b1, a4567, 0);
			c50 = vfmaq_lane_f32(c50, b0, a4567, 1);
			c51 = vfmaq_lane_f32(c51, b1, a4567, 1);
			c60 = vfmaq_lane_f32(c60, b0, a4567, 2);
			c61 = vfmaq_lane_f32(c61, b1, a4567, 2);
			c70 = vfmaq_lane_f32(c70, b0, a4567, 3);
			c71 = vfmaq_lane_f32(c71, b1, a4567, 3);
		}

		// ---- write back --------------------------------------------------------
		vst1q_f32(C + 0*ldc + 0, c00);
		vst1q_f32(C + 0*ldc + 4, c01);
		vst1q_f32(C + 1*ldc + 0, c10);
		vst1q_f32(C + 1*ldc + 4, c11);
		vst1q_f32(C + 2*ldc + 0, c20);
		vst1q_f32(C + 2*ldc + 4, c21);
		vst1q_f32(C + 3*ldc + 0, c30);
		vst1q_f32(C + 3*ldc + 4, c31);
		vst1q_f32(C + 4*ldc + 0, c40);
		vst1q_f32(C + 4*ldc + 4, c41);
		vst1q_f32(C + 5*ldc + 0, c50);
		vst1q_f32(C + 5*ldc + 4, c51);
		vst1q_f32(C + 6*ldc + 0, c60);
		vst1q_f32(C + 6*ldc + 4, c61);
		vst1q_f32(C + 7*ldc + 0, c70);
		vst1q_f32(C + 7*ldc + 4, c71);
	}

	// ===========================================================================
	// Driver: C += A * B   (A: MxK, B: KxN row-major, C: MxN)
	// ===========================================================================
	void MatrixMulAdd(const float* RESTRICT A,
					  const float* RESTRICT B,
					  float* RESTRICT C,
					  const int M, const int N, const int K)
	{
		const int lda = K;
		const int ldb = N;
		const int ldc = N;

		const int M_blk = M - (M % 8);
		const int N_blk = N - (N % 8);

		for (int i = 0; i < M_blk; i += 8)
		{
			int j = 0;
			for (; j < N_blk; j += 8)
			{
				neon_gemm_microkernel_8x8(A + i*lda,       lda,
										  B + j,           ldb,
										  C + i*ldc + j,   ldc,
										  K);
			}

			// ---- N remainder: columns [N_blk, N) for this 8-row strip --------
			// (drop this block if N % 8 == 0 is guaranteed, like the original)
			for (int r = 0; r < 8; ++r)
			{
				for (int k = 0; k < K; ++k)
				{
					const float  a_val = A[(i+r)*lda + k];
					const float* b_row = B + k*ldb;
					float*       c_row = C + (i+r)*ldc;
					for (j = N_blk; j + 4 <= N; j += 4)
					{
						float32x4_t c = vld1q_f32(c_row + j);
						float32x4_t b = vld1q_f32(b_row + j);
						vst1q_f32(c_row + j, vfmaq_n_f32(c, b, a_val));
					}
					for (; j < N; ++j)
						c_row[j] += a_val * b_row[j];
				}
			}
		}

		// ---- M remainder: rows [M_blk, M), NEON over columns ------------------
		for (int i = M_blk; i < M; ++i)
		{
			for (int k = 0; k < K; ++k)
			{
				const float  a_val = A[i*lda + k];
				const float* b_row = B + k*ldb;
				float*       c_row = C + i*ldc;
				int j = 0;
				for (; j + 4 <= N; j += 4)
				{
					float32x4_t c = vld1q_f32(c_row + j);
					float32x4_t b = vld1q_f32(b_row + j);
					vst1q_f32(c_row + j, vfmaq_n_f32(c, b, a_val));
				}
				for (; j < N; ++j)
					c_row[j] += a_val * b_row[j];
			}
		}
	}

} // neon_v2*/
//-----------------------------------------------------------------------------


namespace neon_v3
{
	// C[0:8, 0:8] += A[0:8, 0:K] * B[0:K, 0:8]
	//
	// A, B, and C are row-major.
	// No allocation is performed.
	static inline void neon_gemm_microkernel_8x8(
		const float* RESTRICT A, int lda,
		const float* RESTRICT B, int ldb,
		float* RESTRICT C, int ldc,
		int K)
	{
		// Two 128-bit vectors per output row: columns 0..3 and 4..7.
		float32x4_t c00 = vld1q_f32(C + 0 * ldc + 0);
		float32x4_t c01 = vld1q_f32(C + 0 * ldc + 4);

		float32x4_t c10 = vld1q_f32(C + 1 * ldc + 0);
		float32x4_t c11 = vld1q_f32(C + 1 * ldc + 4);

		float32x4_t c20 = vld1q_f32(C + 2 * ldc + 0);
		float32x4_t c21 = vld1q_f32(C + 2 * ldc + 4);

		float32x4_t c30 = vld1q_f32(C + 3 * ldc + 0);
		float32x4_t c31 = vld1q_f32(C + 3 * ldc + 4);

		float32x4_t c40 = vld1q_f32(C + 4 * ldc + 0);
		float32x4_t c41 = vld1q_f32(C + 4 * ldc + 4);

		float32x4_t c50 = vld1q_f32(C + 5 * ldc + 0);
		float32x4_t c51 = vld1q_f32(C + 5 * ldc + 4);

		float32x4_t c60 = vld1q_f32(C + 6 * ldc + 0);
		float32x4_t c61 = vld1q_f32(C + 6 * ldc + 4);

		float32x4_t c70 = vld1q_f32(C + 7 * ldc + 0);
		float32x4_t c71 = vld1q_f32(C + 7 * ldc + 4);

		for (int k = 0; k < K; ++k)
		{
			const float* bk = B + k * ldb;

			const float32x4_t b0 = vld1q_f32(bk + 0);
			const float32x4_t b1 = vld1q_f32(bk + 4);

			// vfmaq_n_f32 performs vector + vector * scalar.
			c00 = vfmaq_n_f32(c00, b0, A[0 * lda + k]);
			c01 = vfmaq_n_f32(c01, b1, A[0 * lda + k]);

			c10 = vfmaq_n_f32(c10, b0, A[1 * lda + k]);
			c11 = vfmaq_n_f32(c11, b1, A[1 * lda + k]);

			c20 = vfmaq_n_f32(c20, b0, A[2 * lda + k]);
			c21 = vfmaq_n_f32(c21, b1, A[2 * lda + k]);

			c30 = vfmaq_n_f32(c30, b0, A[3 * lda + k]);
			c31 = vfmaq_n_f32(c31, b1, A[3 * lda + k]);

			c40 = vfmaq_n_f32(c40, b0, A[4 * lda + k]);
			c41 = vfmaq_n_f32(c41, b1, A[4 * lda + k]);

			c50 = vfmaq_n_f32(c50, b0, A[5 * lda + k]);
			c51 = vfmaq_n_f32(c51, b1, A[5 * lda + k]);

			c60 = vfmaq_n_f32(c60, b0, A[6 * lda + k]);
			c61 = vfmaq_n_f32(c61, b1, A[6 * lda + k]);

			c70 = vfmaq_n_f32(c70, b0, A[7 * lda + k]);
			c71 = vfmaq_n_f32(c71, b1, A[7 * lda + k]);
		}

		vst1q_f32(C + 0 * ldc + 0, c00);
		vst1q_f32(C + 0 * ldc + 4, c01);

		vst1q_f32(C + 1 * ldc + 0, c10);
		vst1q_f32(C + 1 * ldc + 4, c11);

		vst1q_f32(C + 2 * ldc + 0, c20);
		vst1q_f32(C + 2 * ldc + 4, c21);

		vst1q_f32(C + 3 * ldc + 0, c30);
		vst1q_f32(C + 3 * ldc + 4, c31);

		vst1q_f32(C + 4 * ldc + 0, c40);
		vst1q_f32(C + 4 * ldc + 4, c41);

		vst1q_f32(C + 5 * ldc + 0, c50);
		vst1q_f32(C + 5 * ldc + 4, c51);

		vst1q_f32(C + 6 * ldc + 0, c60);
		vst1q_f32(C + 6 * ldc + 4, c61);

		vst1q_f32(C + 7 * ldc + 0, c70);
		vst1q_f32(C + 7 * ldc + 4, c71);
	}


	// Handles 1..7 remaining columns for one output row using safe,
	// non-overreading NEON loads/stores where possible.
	static inline void neon_row_tail(
		const float* RESTRICT Arow,
		const float* RESTRICT B,
		float* RESTRICT Crow,
		int ldb,
		int K,
		int columns)
	{
		int j = 0;

		// Handle four columns at once.
		if (columns >= 4)
		{
			float32x4_t c = vld1q_f32(Crow);

			for (int k = 0; k < K; ++k)
				c = vfmaq_n_f32(c, vld1q_f32(B + k * ldb), Arow[k]);

			vst1q_f32(Crow, c);
			j = 4;
		}

		// Handle the remaining 0..3 columns without reading out of bounds.
		for (; j < columns; ++j)
		{
			float sum = Crow[j];

			for (int k = 0; k < K; ++k)
				sum += Arow[k] * B[k * ldb + j];

			Crow[j] = sum;
		}
	}


	// Computes C += A * B.
	//
	// A: M x K, row-major
	// B: K x N, row-major
	// C: M x N, row-major
	//
	// No heap or stack tile buffers are allocated.
	void MatrixMulAdd(
		const float* RESTRICT A,
		const float* RESTRICT B,
		float* RESTRICT C,
		const int M,
		const int N,
		const int K)
	{
		const int lda = K;
		const int ldb = N;
		const int ldc = N;

		const int M8 = M & ~7;
		const int N8 = N & ~7;

		// Complete 8x8 output tiles.
		for (int i = 0; i < M8; i += 8)
		{
			const float* A_tile = A + i * lda;

			for (int j = 0; j < N8; j += 8)
			{
				neon_gemm_microkernel_8x8(
					A_tile, lda,
					B + j, ldb,
					C + i * ldc + j, ldc,
					K);
			}

			// Remaining columns for each row of this 8-row block.
			if (N8 < N)
			{
				const int remaining_columns = N - N8;

				for (int r = 0; r < 8; ++r)
				{
					neon_row_tail(
						A + (i + r) * lda,
						B + N8,
						C + (i + r) * ldc + N8,
						ldb,
						K,
						remaining_columns);
				}
			}
		}

		// Remaining rows. Process full groups of eight columns with NEON.
		for (int i = M8; i < M; ++i)
		{
			const float* Arow = A + i * lda;
			float* Crow = C + i * ldc;

			for (int j = 0; j < N8; j += 8)
			{
				float32x4_t c0 = vld1q_f32(Crow + j + 0);
				float32x4_t c1 = vld1q_f32(Crow + j + 4);

				for (int k = 0; k < K; ++k)
				{
					const float* bk = B + k * ldb + j;
					const float a = Arow[k];

					c0 = vfmaq_n_f32(c0, vld1q_f32(bk + 0), a);
					c1 = vfmaq_n_f32(c1, vld1q_f32(bk + 4), a);
				}

				vst1q_f32(Crow + j + 0, c0);
				vst1q_f32(Crow + j + 4, c1);
			}

			if (N8 < N)
			{
				neon_row_tail(
					Arow,
					B + N8,
					Crow + N8,
					ldb,
					K,
					N - N8);
			}
		}
	}

} // neon_v3
//-----------------------------------------------------------------------------
#endif // NEON


#if AE_SIMD_SVE > 1
namespace sve_v1
{
	// ===========================================================================
	// SVE2 SGEMM microkernel: 8 rows x 2*VL columns  (VL = svcntw() floats)
	//
	// Register budget (32 scalable z-regs, any VL):
	//    16 x svfloat32_t  accumulators
	//     2 x svfloat32_t  B row
	//     1 x svfloat32_t  broadcast temp (reused)
	//    = ~20/32 live registers -> spill-free, headroom for k-unroll
	//
	// rows    : valid rows, 1..8. Rows >= rows are clamped to row 0 (loads stay
	//           in-bounds) and their results are never stored.
	// pg0/pg1 : column predicates for lanes [0,VL) and [VL,2VL). All-true for
	//           full tiles; partial/all-false at the right edge. Predicated-off
	//           lanes perform NO memory access, so no fault is possible.
	// ===========================================================================
	static inline void sve2_gemm_microkernel_8x2vl(
		const float* RESTRICT A, int lda,
		const float* RESTRICT B, int ldb,
		float* RESTRICT C,       int ldc,
		int K,
		int rows,
		svbool_t pg0, svbool_t pg1)
	{
		const uint64_t vl   = svcntw();
		const svbool_t pFMA = svptrue_b32();   // FMAs unpredicated: tail lanes of
											   // the /z loads are 0, so they just
											   // compute inert zeros (see notes)

		// Clamp out-of-range rows to row 0. With rows==8 (main loop) the compiler
		// constant-folds all of this away after inlining.
		const int r1 = (rows > 1) ? 1 : 0, r2 = (rows > 2) ? 2 : 0;
		const int r3 = (rows > 3) ? 3 : 0, r4 = (rows > 4) ? 4 : 0;
		const int r5 = (rows > 5) ? 5 : 0, r6 = (rows > 6) ? 6 : 0;
		const int r7 = (rows > 7) ? 7 : 0;

		const float* a0 = A + 0 *lda;  const float* a1 = A + r1*lda;
		const float* a2 = A + r2*lda;  const float* a3 = A + r3*lda;
		const float* a4 = A + r4*lda;  const float* a5 = A + r5*lda;
		const float* a6 = A + r6*lda;  const float* a7 = A + r7*lda;

		float* c0 = C + 0 *ldc;  float* c1 = C + r1*ldc;
		float* c2 = C + r2*ldc;  float* c3 = C + r3*ldc;
		float* c4 = C + r4*ldc;  float* c5 = C + r5*ldc;
		float* c6 = C + r6*ldc;  float* c7 = C + r7*ldc;

		// ---- 16 scalable accumulators -----------------------------------------
		svfloat32_t c00 = svld1(pg0, c0);
		svfloat32_t c01 = svld1(pg1, c0 + vl);
		svfloat32_t c10 = svld1(pg0, c1);
		svfloat32_t c11 = svld1(pg1, c1 + vl);
		svfloat32_t c20 = svld1(pg0, c2);
		svfloat32_t c21 = svld1(pg1, c2 + vl);
		svfloat32_t c30 = svld1(pg0, c3);
		svfloat32_t c31 = svld1(pg1, c3 + vl);
		svfloat32_t c40 = svld1(pg0, c4);
		svfloat32_t c41 = svld1(pg1, c4 + vl);
		svfloat32_t c50 = svld1(pg0, c5);
		svfloat32_t c51 = svld1(pg1, c5 + vl);
		svfloat32_t c60 = svld1(pg0, c6);
		svfloat32_t c61 = svld1(pg1, c6 + vl);
		svfloat32_t c70 = svld1(pg0, c7);
		svfloat32_t c71 = svld1(pg1, c7 + vl);

		for (int k = 0; k < K; ++k)
		{
			const float* bk = B + (size_t)k * ldb;

			// ---- B: 2 scalable vectors (2*VL contiguous floats) ---------------
			svfloat32_t b0 = svld1(pg0, bk);
			svfloat32_t b1 = svld1(pg1, bk + vl);

			// ---- rank-1 update: svdup lowers to LD1RW broadcast-load ----------
			svfloat32_t a;
			a = svdup_n_f32(a0[k]);
			c00 = svmla_f32_x(pFMA, c00, b0, a);
			c01 = svmla_f32_x(pFMA, c01, b1, a);

			a = svdup_n_f32(a1[k]);
			c10 = svmla_f32_x(pFMA, c10, b0, a);
			c11 = svmla_f32_x(pFMA, c11, b1, a);

			a = svdup_n_f32(a2[k]);
			c20 = svmla_f32_x(pFMA, c20, b0, a);
			c21 = svmla_f32_x(pFMA, c21, b1, a);

			a = svdup_n_f32(a3[k]);
			c30 = svmla_f32_x(pFMA, c30, b0, a);
			c31 = svmla_f32_x(pFMA, c31, b1, a);

			a = svdup_n_f32(a4[k]);
			c40 = svmla_f32_x(pFMA, c40, b0, a);
			c41 = svmla_f32_x(pFMA, c41, b1, a);

			a = svdup_n_f32(a5[k]);
			c50 = svmla_f32_x(pFMA, c50, b0, a);
			c51 = svmla_f32_x(pFMA, c51, b1, a);

			a = svdup_n_f32(a6[k]);
			c60 = svmla_f32_x(pFMA, c60, b0, a);
			c61 = svmla_f32_x(pFMA, c61, b1, a);

			a = svdup_n_f32(a7[k]);
			c70 = svmla_f32_x(pFMA, c70, b0, a);
			c71 = svmla_f32_x(pFMA, c71, b1, a);
		}

		// ---- write back (row 0 always; rest guarded, folded away when rows==8) --
		svst1(pg0, c0,      c00);
		svst1(pg1, c0 + vl, c01);
		if (rows > 1) { svst1(pg0, c1, c10); svst1(pg1, c1 + vl, c11); }
		if (rows > 2) { svst1(pg0, c2, c20); svst1(pg1, c2 + vl, c21); }
		if (rows > 3) { svst1(pg0, c3, c30); svst1(pg1, c3 + vl, c31); }
		if (rows > 4) { svst1(pg0, c4, c40); svst1(pg1, c4 + vl, c41); }
		if (rows > 5) { svst1(pg0, c5, c50); svst1(pg1, c5 + vl, c51); }
		if (rows > 6) { svst1(pg0, c6, c60); svst1(pg1, c6 + vl, c61); }
		if (rows > 7) { svst1(pg0, c7, c70); svst1(pg1, c7 + vl, c71); }
	}

	// ===========================================================================
	// Driver: C += A * B   (A: MxK, B: KxN row-major, C: MxN)
	// No remainder loops anywhere: predicates handle column tails, row clamping
	// handles the row tail.
	// ===========================================================================
	void MatrixMulAdd(const float* RESTRICT A,
					  const float* RESTRICT B,
					  float* RESTRICT C,
					  const int M, const int N, const int K)
	{
		const int lda = K;
		const int ldb = N;
		const int ldc = N;

		const int vl = (int)svcntw();
		const int tn = 2 * vl;        // kernel column tile = 2*VL
		const int mt = M & ~7;        // bound for full 8-row blocks

		int i = 0;
		for (; i < mt; i += 8)                       // full 8-row blocks
		{
			for (int j = 0; j < N; j += tn)
			{
				const svbool_t pg0 = svwhilelt_b32((uint64_t)j,       (uint64_t)N);
				const svbool_t pg1 = svwhilelt_b32((uint64_t)(j + vl),(uint64_t)N);

				sve2_gemm_microkernel_8x2vl(A + (size_t)i*lda,     lda,
											B + j,                 ldb,
											C + (size_t)i*ldc + j, ldc,
											K, 8, pg0, pg1);
			}
		}

		if (i < M)                                   // tail block: 1..7 rows
		{
			const int rows = M - i;
			for (int j = 0; j < N; j += tn)
			{
				const svbool_t pg0 = svwhilelt_b32((uint64_t)j,       (uint64_t)N);
				const svbool_t pg1 = svwhilelt_b32((uint64_t)(j + vl),(uint64_t)N);

				sve2_gemm_microkernel_8x2vl(A + (size_t)i*lda,     lda,
											B + j,                 ldb,
											C + (size_t)i*ldc + j, ldc,
											K, rows, pg0, pg1);
			}
		}
	}

} // sve_v1
//-----------------------------------------------------------------------------
#endif // SVE2


#if 0 //AE_SIMD_SME
namespace sme_v1
{
	// ---------------------------------------------------------------------------
	// Configuration
	// ---------------------------------------------------------------------------
	#define SME_GEMM_KC     128   // K-panel depth. Stack panel = KC*SVL floats:
								  // 2KB @128-bit SVL, 4KB @256-bit, 8KB @512-bit
	#define SME_GEMM_ALIGN  16    // required alignment of base ptrs & row starts
								  // (set to 4 to accept any float alignment)

	typedef enum
	{
		SME_GEMM_OK        =  0,
		SME_GEMM_ERR_NULL  = -1,    // A, B or C is NULL
		SME_GEMM_ERR_DIM   = -2,    // M, N or K is negative
		SME_GEMM_ERR_ALIGN = -3,    // base pointer or leading dim misaligned
	} sme_gemm_status;

	// ---------------------------------------------------------------------------
	// Validate sizes + alignment.
	// lda = K, ldb = ldc = N, so row r starts at (base + r*ld): every row start
	// stays SME_GEMM_ALIGN-aligned iff K%4==0 and N%4==0 (given aligned bases).
	// No constraint on M; N/K need NOT be multiples of SVL (predication + zero
	// padding handle any tail). Alignment is a performance POLICY only -- SVE/SME
	// loads tolerate any alignment, so relaxing SME_GEMM_ALIGN is always safe.
	// ---------------------------------------------------------------------------
	static sme_gemm_status sme_gemm_check(const float* A, const float* B,
										  const float* C,
										  int M, int N, int K)
	{
		if (A == NULL || B == NULL || C == NULL)     return SME_GEMM_ERR_NULL;
		if (M < 0 || N < 0 || K < 0)                 return SME_GEMM_ERR_DIM;

		const uintptr_t or3 = (uintptr_t)A | (uintptr_t)B | (uintptr_t)C;
		if (or3 % SME_GEMM_ALIGN != 0)               return SME_GEMM_ERR_ALIGN;
		if (K % (SME_GEMM_ALIGN / 4) != 0)           return SME_GEMM_ERR_ALIGN;
		if (N % (SME_GEMM_ALIGN / 4) != 0)           return SME_GEMM_ERR_ALIGN;

		return SME_GEMM_OK;
	}

	// ---------------------------------------------------------------------------
	// Pack one kc-deep LHS panel: dst[k*vl + r] = A[r*lda + k]
	// Zero-padded for r >= rows so dead rows contribute exact zeros to FMOPA.
	// Cost amortized over the N/SVL column tiles that reuse this panel.
	// ---------------------------------------------------------------------------
	__attribute__((arm_streaming_compatible))
	static void pack_lhs_panel(float* RESTRICT dst,
							   const float* RESTRICT A, int lda,
							   int rows, int kc, int vl)
	{
		for (int k = 0; k < kc; ++k)
		{
			float* d = dst + (size_t)k * vl;
			int r = 0;
			for (; r < rows; ++r) d[r] = A[(size_t)r*lda + k];
			for (; r < vl;   ++r) d[r] = 0.0f;
		}
	}

	// ---------------------------------------------------------------------------
	// SME microkernel: SVL x SVL block of C += A*B in ZA tile 0.
	// Runs inside the driver's streaming session (arm_shared_za); ZA persists
	// across calls, so each tile starts with svzero_za().
	//   rows : 1..SVL valid rows (tail rows zero-padded in Apack, never stored)
	//   pn   : svwhilelt_b32(j, N); masked-off lanes touch no memory
	// ---------------------------------------------------------------------------
	__attribute__((arm_streaming, arm_shared_za))
	static void sme_gemm_microkernel(
		const float* RESTRICT Apack,          // kc x SVL packed LHS panel
		const float* RESTRICT B, int ldb, int n0,
		float* RESTRICT C,       int ldc, int m0,
		int kc, int rows, svbool_t pn)
	{
		const svbool_t pAll = svptrue_b32();
		const uint32_t vl   = svcntw();       // == SVL_s while streaming
		float*         Cblk = C + (size_t)m0*ldc + n0;

		svzero_za();

		// Preload C to get C += A*B semantics.
		for (uint32_t r = 0; r < (uint32_t)rows; ++r)
			svld1_hor_za32(0, r, pn, Cblk + (size_t)r*ldc);

		const float* ap = Apack;
		const float* bp = B + n0;

		int k = 0;
		for (; k + 2 <= kc; k += 2)
		{
			svfloat32_t a0 = svld1(pAll, ap);
			svfloat32_t a1 = svld1(pAll, ap + vl);
			svfloat32_t b0 = svld1(pn,   bp);
			svfloat32_t b1 = svld1(pn,   bp + ldb);

			// ZA[i][j] += a[i]*b[j]; padded A rows and masked B lanes are exact
			// zeros, so FMOPA runs unpredicated.
			svfmopa_za32_f32_m(0, pAll, pAll, a0, b0);
			svfmopa_za32_f32_m(0, pAll, pAll, a1, b1);

			ap += 2*vl;
			bp += 2*(size_t)ldb;
		}
		if (k < kc)
		{
			svfloat32_t a0 = svld1(pAll, ap);
			svfloat32_t b0 = svld1(pn,   bp);
			svfmopa_za32_f32_m(0, pAll, pAll, a0, b0);
		}

		for (uint32_t r = 0; r < (uint32_t)rows; ++r)
			svst1_hor_za32(0, r, pn, Cblk + (size_t)r*ldc);
	}

	// ---------------------------------------------------------------------------
	// Driver: C += A * B  (A: MxK, B: KxN, C: MxN, all row-major)
	// Single streaming session; K processed in KC-deep stack panels. No heap.
	// ---------------------------------------------------------------------------
	__attribute__((arm_locally_streaming, arm_new_za))
	sme_gemm_status MatrixMulAdd(const float* RESTRICT A,
								 const float* RESTRICT B,
								 float* RESTRICT C,
								 const int M, const int N, const int K)
	{
		const sme_gemm_status st = sme_gemm_check(A, B, C, M, N, K);
		if (st != SME_GEMM_OK) return st;
		if (M == 0 || N == 0 || K == 0) return SME_GEMM_OK;   // nothing to do

		const int lda = K, ldb = N, ldc = N;

		// Streaming vector length (SVL may exceed the plain SVE length).
		const int vl = (int)svcntsw();

		// Stack-resident panel -- sized by the actual SVL, bounded by KC*64*4.
		alignas(64) float Apack[(size_t)SME_GEMM_KC * (size_t)vl];

		for (int i = 0; i < M; i += vl)
		{
			const int rows = (M - i < vl) ? M - i : vl;

			for (int k0 = 0; k0 < K; k0 += SME_GEMM_KC)
			{
				const int kc = (K - k0 < SME_GEMM_KC) ? K - k0 : SME_GEMM_KC;

				pack_lhs_panel(Apack, A + (size_t)i*lda + k0, lda, rows, kc, vl);

				const float* Bk = B + (size_t)k0*ldb;
				for (int j = 0; j < N; j += vl)
				{
					const svbool_t pn = svwhilelt_b32((uint64_t)j, (uint64_t)N);
					sme_gemm_microkernel(Apack, Bk, ldb, j,
										 C, ldc, i, kc, rows, pn);
				}
			}
		}
		return SME_GEMM_OK;
	}

} // sme_v1
//-----------------------------------------------------------------------------
#endif // SME

//-----------------------------------------------------------------------------


	static void  SimdMatTest (const uint M, const uint N, const uint K, const usize max_iter, ECoreType coreType)
	{
		const bool	col_maj		= false;

		const usize	n_muls		= M * N * K;
		const usize	n_adds		= M * N * (K - 1);
		const usize	flops		= n_muls + n_adds;
		const usize	data_size	= sizeof(float) * (M * K + K * N + 2 * M * N);	// RAM read bandwidth

		const auto	Print = [flops, data_size](secondsd dt)
		{{
			return	ToStringSfx( double(flops) / dt.count() ) << "FLOPS | " <<
					ToStringSfx( double(data_size) / dt.count() ) << "B/s";
		}};

		DynUntypedStorage	A;	A.Alloc( SizeOf<float> * M * K,	64_b, null );
		DynUntypedStorage	B;	B.Alloc( SizeOf<float> * K * N,	64_b, null );
		DynUntypedStorage	C;	C.Alloc( SizeOf<float> * M * N, 64_b, null );
		DynUntypedStorage	R;	R.Alloc( SizeOf<float> * M * N, 64_b, null );

		const auto	Reset = [&] ()
		{{
			FillWithLinearData<float>( A, 0.1f,  2.f );
			FillWithLinearData<float>( B, 0.4f, -1.f );
			FillWithLinearData<float>( C, 0.8f,  3.f );
		}};

		IntervalProfiler	profiler{ "GEMM MulAdd "s <<ToString(M)<<'x'<<ToString(N)<<'x'<<ToString(K) << " on " << ToString(coreType)<<" core" };

	#if AE_SIMD_AVX >= 2
		{
			profiler.BeginTest( "AVX2 v1", Print );
			Reset();
			for (usize i = 0; i < max_iter; ++i)
			{
				MemCopy( OUT R.Data(), R.Size(), C.Data(), C.Size() );

				profiler.BeginIteration();
				avx2_v1::MatrixMulAdd( A.Ptr<float>(), B.Ptr<float>(), R.Ptr<float>(), M, N, K );
				profiler.EndIteration();
			}
			profiler.EndTest();
			TestMatMulAdd<float>( A, B, C, R, MatrixDim{K,M}, MatrixDim{N,K}, MatrixDim{N,M}, col_maj, "AVX2 v1" );
		}{
			profiler.BeginTest( "AVX2 v2", Print );
			Reset();
			for (usize i = 0; i < max_iter; ++i)
			{
				MemCopy( OUT R.Data(), R.Size(), C.Data(), C.Size() );

				profiler.BeginIteration();
				avx2_v2::MatrixMulAdd( A.Ptr<float>(), B.Ptr<float>(), R.Ptr<float>(), M, N, K );
				profiler.EndIteration();
			}
			profiler.EndTest();
			TestMatMulAdd<float>( A, B, C, R, MatrixDim{K,M}, MatrixDim{N,K}, MatrixDim{N,M}, col_maj, "AVX2 v2" );
		}{
			profiler.BeginTest( "AVX2 v3", Print );
			Reset();
			for (usize i = 0; i < max_iter; ++i)
			{
				MemCopy( OUT R.Data(), R.Size(), C.Data(), C.Size() );

				profiler.BeginIteration();
				avx2_v3::matrix_mul_add_avx2( A.Ptr<float>(), B.Ptr<float>(), R.Ptr<float>(), M, N, K );
				profiler.EndIteration();
			}
			profiler.EndTest();
			TestMatMulAdd<float>( A, B, C, R, MatrixDim{K,M}, MatrixDim{N,K}, MatrixDim{N,M}, col_maj, "AVX2 v3" );
		}{
			profiler.BeginTest( "AVX2 v4", Print );
			Reset();
			for (usize i = 0; i < max_iter; ++i)
			{
				MemCopy( OUT R.Data(), R.Size(), C.Data(), C.Size() );

				profiler.BeginIteration();
				avx2_v4::MatrixMulAdd( A.Ptr<float>(), B.Ptr<float>(), R.Ptr<float>(), M, N, K );
				profiler.EndIteration();
			}
			profiler.EndTest();
			TestMatMulAdd<float>( A, B, C, R, MatrixDim{K,M}, MatrixDim{N,K}, MatrixDim{N,M}, col_maj, "AVX2 v4" );
		}{
			profiler.BeginTest( "AVX2 v5", Print );
			Reset();
			for (usize i = 0; i < max_iter; ++i)
			{
				MemCopy( OUT R.Data(), R.Size(), C.Data(), C.Size() );

				profiler.BeginIteration();
				avx2_v5::MatrixMulAdd( A.Ptr<float>(), B.Ptr<float>(), R.Ptr<float>(), M, N, K );
				profiler.EndIteration();
			}
			profiler.EndTest();
			TestMatMulAdd<float>( A, B, C, R, MatrixDim{K,M}, MatrixDim{N,K}, MatrixDim{N,M}, col_maj, "AVX2 v5" );
		}
	#endif // AVX
	#if AE_SIMD_NEON
		{
			profiler.BeginTest( "NEON v1", Print );
			Reset();
			for (usize i = 0; i < max_iter; ++i)
			{
				MemCopy( OUT R.Data(), R.Size(), C.Data(), C.Size() );

				profiler.BeginIteration();
				neon_v1::MatrixMulAdd( A.Ptr<float>(), B.Ptr<float>(), R.Ptr<float>(), M, N, K );
				profiler.EndIteration();
			}
			profiler.EndTest();
			TestMatMulAdd<float>( A, B, C, R, MatrixDim{K,M}, MatrixDim{N,K}, MatrixDim{N,M}, col_maj, "NEON v1" );
		}
		/*{
			profiler.BeginTest( "NEON v2", Print );
			Reset();
			for (usize i = 0; i < max_iter; ++i)
			{
				MemCopy( OUT R.Data(), R.Size(), C.Data(), C.Size() );

				profiler.BeginIteration();
				neon_v2::MatrixMulAdd( A.Ptr<float>(), B.Ptr<float>(), R.Ptr<float>(), M, N, K );
				profiler.EndIteration();
			}
			profiler.EndTest();
			TestMatMulAdd<float>( A, B, C, R, MatrixDim{K,M}, MatrixDim{N,K}, MatrixDim{N,M}, col_maj, "NEON v2" );
		}*/
		{
			profiler.BeginTest( "NEON v3", Print );
			Reset();
			for (usize i = 0; i < max_iter; ++i)
			{
				MemCopy( OUT R.Data(), R.Size(), C.Data(), C.Size() );

				profiler.BeginIteration();
				neon_v3::MatrixMulAdd( A.Ptr<float>(), B.Ptr<float>(), R.Ptr<float>(), M, N, K );
				profiler.EndIteration();
			}
			profiler.EndTest();
			TestMatMulAdd<float>( A, B, C, R, MatrixDim{K,M}, MatrixDim{N,K}, MatrixDim{N,M}, col_maj, "NEON v3" );
		}
	#endif // NEON
	#if AE_SIMD_SVE > 1
		{
			profiler.BeginTest( "SVE v1", Print );
			Reset();
			for (usize i = 0; i < max_iter; ++i)
			{
				MemCopy( OUT R.Data(), R.Size(), C.Data(), C.Size() );

				profiler.BeginIteration();
				sve_v1::MatrixMulAdd( A.Ptr<float>(), B.Ptr<float>(), R.Ptr<float>(), M, N, K );
				profiler.EndIteration();
			}
			profiler.EndTest();
			TestMatMulAdd<float>( A, B, C, R, MatrixDim{K,M}, MatrixDim{N,K}, MatrixDim{N,M}, col_maj, "SVE v1" );
		}
	#endif // SVE
	#if 0 //AE_SIMD_SME
		{
			profiler.BeginTest( "SME v1", Print );
			Reset();
			for (usize i = 0; i < max_iter; ++i)
			{
				MemCopy( OUT R.Data(), R.Size(), C.Data(), C.Size() );

				profiler.BeginIteration();
				CHECK( sme_v1::MatrixMulAdd( A.Ptr<float>(), B.Ptr<float>(), R.Ptr<float>(), M, N, K ) == sme_v1::SME_GEMM_OK );
				profiler.EndIteration();
			}
			profiler.EndTest();
			TestMatMulAdd<float>( A, B, C, R, MatrixDim{K,M}, MatrixDim{N,K}, MatrixDim{N,M}, col_maj, "SME v1" );
		}
	#endif // SME
		{
			profiler.BeginTest( "AE GEMM", Print );
			Reset();
			for (usize i = 0; i < max_iter; ++i)
			{
				MemCopy( OUT R.Data(), R.Size(), C.Data(), C.Size() );

				profiler.BeginIteration();
				GEMM::MulAdd(	LargeMatrixView<const float,	EMatrixLayout::RowMajor>{ A.Data<float>(), A.Size(), M, K },
								LargeMatrixView<const float,	EMatrixLayout::RowMajor>{ B.Data<float>(), B.Size(), K, N },
								INOUT LargeMatrixView<float,	EMatrixLayout::RowMajor>{ R.Data<float>(), R.Size(), M, N });
				profiler.EndIteration();
			}
			profiler.EndTest();
			TestMatMulAdd<float>( A, B, C, R, MatrixDim{K,M}, MatrixDim{N,K}, MatrixDim{N,M}, col_maj, "AE GEMM" );
		}
	}

	static void  SimdMat_Test1 (ECoreType coreType)
	{
		const uint	M			= 1024;
		const uint	N			= 1024;
		const uint	K			= 1024;
		#ifdef AE_DEBUG
		const usize	max_iter	= 1;
		#else
		const usize	max_iter	= coreType > ECoreType::EnergyEfficient ? 20 : 10;
		#endif

		SimdMatTest( M, N, K, max_iter, coreType );
	}

	static void  SimdMat_Test2 (ECoreType coreType)
	{
		const uint	M			= 128;
		const uint	N			= 128;
		const uint	K			= 128;
		#ifdef AE_DEBUG
		const usize	max_iter	= 1;
		#else
		const usize	max_iter	= coreType > ECoreType::EnergyEfficient ? 10'000 : 1'000;
		#endif

		SimdMatTest( M, N, K, max_iter, coreType );
	}

} // namespace


extern void PerfTest_SimdGEMM ()
{
	ForEachCoreType(
		[&] (auto &core, Function<void()> setAffinity)
		{
			setAffinity();

			SimdMat_Test1( core.type );
			SimdMat_Test2( core.type );
		});

	TEST_PASSED();
}
