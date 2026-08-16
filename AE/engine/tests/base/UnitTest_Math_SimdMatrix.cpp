// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "UnitTest_Common.h"

namespace
{
	template <typename T, EMatrixLayout L>
	void  FillWithLinearData (INOUT LargeMatrixStorage<T,L> &buf, float scale, float bias)
	{
		T*			arr		= buf.Data();
		const usize	count	= buf.Dimension().Size();

		for (usize i = 0; i < count; ++i) {
			arr[i] = T( float(i) * scale + bias );
		}
	}

	template <typename T>
	void  FillWithLinearData (INOUT LargeVectorStorage<T> &buf, float scale, float bias)
	{
		T*			arr		= buf.Data();
		const usize	count	= buf.Length();

		for (usize i = 0; i < count; ++i) {
			arr[i] = T( float(i) * scale + bias );
		}
	}


	static void  TestMatMulAdd (ArrayView<float> inputA, ArrayView<float> inputB, ArrayView<float> inputC, ArrayView<float> refOutput,
								const MatrixDim dimA, const MatrixDim dimB, const MatrixDim dimC, const bool columnMajor)
	{
		TEST( inputA.size() == dimA.rows * dimA.columns );
		TEST( inputB.size() == dimB.rows * dimB.columns );
		TEST( inputC.size() == dimC.rows * dimC.columns );

		TEST( dimA.columns	== dimB.rows );
		TEST( dimA.rows		== dimC.rows );
		TEST( dimB.columns	== dimC.columns );

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

	//	str << "\n| C,R | expected | SIMD output | error % |\n";

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

		str << "max error: " << ToString( max_err, 2 ) << "%, avr: " << ToString( avr_err, 2 ) << "%\n";
	//	str << "----------------------------------------\n\n";
		AE_LOGI( str );
		CHECK( max_err < 1.f );
	}

	template <typename T, EMatrixLayout Layout>
	static void  TestMatMulAdd2 (LargeMatrixView<const T, Layout> inputA,
								 LargeMatrixView<const T, Layout> inputB,
								 LargeMatrixView<const T, Layout> inputC,
								 LargeMatrixView<const T, Layout> refOutput)
	{
		TestMatMulAdd( ArrayView<T>{inputA}, ArrayView<T>{inputB}, ArrayView<T>{inputC}, ArrayView<T>{refOutput},
					   inputA.Dimension(), inputB.Dimension(), inputC.Dimension(),
					   Layout == EMatrixLayout::ColumnMajor );
	}
//-----------------------------------------------------------------------------



	static void  TestVecMatMulAdd (ArrayView<float> inputA, ArrayView<float> inputB, ArrayView<float> inputC, ArrayView<float> refOutput,
								   const uint rowA, const MatrixDim dimB, const uint rowC, const bool columnMajor)
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

		str << "\n| I | expected | SIMD output | error % |\n";

		for (uint i = 0; i < rowC; ++i)
		{
			float	err	= Abs(output[i] - refOutput[i]) * 100.f / Max( Abs(output[i]), 1.0e-5f );
			max_err = Max( max_err, err );
			avr_err += err;

			str << "| " << ToString( i ) << " | " << ToString( output[i], 5 )
				<< " | " << ToString( refOutput[i], 5 ) << " | "
				<< ToString( err, 2 ) << "% |\n";
		}
		avr_err /= float(rowC);

		str << "max error: " << ToString( max_err, 2 ) << "%, avr: " << ToString( avr_err, 2 ) << "%\n";
		//str << "----------------------------------------\n\n";
		AE_LOGI( str );
		CHECK( max_err < 1.f );
	}

	template <typename T, EMatrixLayout Layout>
	static void  TestVecMatMulAdd2 (LargeVectorView<const T>			inputA,
									LargeMatrixView<const T, Layout>	inputB,
									LargeVectorView<const T>			inputC,
									LargeVectorView<const T>			refOutput)
	{
		TestVecMatMulAdd( ArrayView<T>{inputA}, ArrayView<T>{inputB}, ArrayView<T>{inputC}, ArrayView<T>{refOutput},
						  inputA.Length(), inputB.Dimension(), inputC.Length(),
						  Layout == EMatrixLayout::ColumnMajor );
	}
//-----------------------------------------------------------------------------



	static void  SimdMat_Test1 ()
	{
		const uint		M		= 1024;
		const uint		N		= 1024;
		const uint		K		= 1024;
		const Bytes		align	= 64_b;
		constexpr auto	Layout	= EMatrixLayout::RowMajor;

		LargeMatrixStorage< float, Layout >		A { M, K, align };
		LargeMatrixStorage< float, Layout >		B { K, N, align };
		LargeMatrixStorage< float, Layout >		C { M, N, align };
		LargeMatrixStorage< float, Layout >		R { M, N, align };

		FillWithLinearData( A, 0.1f,  2.f );
		FillWithLinearData( B, 0.4f, -1.f );
		FillWithLinearData( C, 0.8f,  3.f );
		MemCopy( OUT R.Data(), R.DataSize(), C.Data(), C.DataSize() );

		TEST( GEMM::IsValidDim( A, B, C ));
		GEMM::MulAdd( A, B, INOUT R );

		TestMatMulAdd2<float, Layout>( A, B, C, R );
	}


	static void  SimdMat_Test2 ()
	{
		const uint		M		= 6*16 + 4;
		const uint		N		= 6*16 + 7;
		const uint		K		= 6*16 + 9;
		const Bytes		align	= 64_b;
		constexpr auto	Layout	= EMatrixLayout::RowMajor;

		LargeMatrixStorage< float, Layout >		A { M, K, align };
		LargeMatrixStorage< float, Layout >		B { K, N, align };
		LargeMatrixStorage< float, Layout >		C { M, N, align };
		LargeMatrixStorage< float, Layout >		R { M, N, align };

		FillWithLinearData( A, 0.1f,  2.f );
		FillWithLinearData( B, 0.4f, -1.f );
		FillWithLinearData( C, 0.8f,  3.f );
		MemCopy( OUT R.Data(), R.DataSize(), C.Data(), C.DataSize() );

		TEST( GEMM::IsValidDim( A, B, C ));
		GEMM::MulAdd( A, B, INOUT R );

		TestMatMulAdd2<float, Layout>( A, B, C, R );
	}


	static void  SimdVecMat_Test1 ()
	{
		const uint		M		= 1024;
		const uint		K		= 512;
		const Bytes		align	= 64_b;
		constexpr auto	Layout	= EMatrixLayout::RowMajor;

		LargeVectorStorage< float >				A { K, align };
		LargeMatrixStorage< float, Layout >		B { M, K, align };
		LargeVectorStorage< float >				C { M, align };
		LargeVectorStorage< float >				R { M, align };

		FillWithLinearData( A, 0.1f,  2.f );
		FillWithLinearData( B, 0.4f, -1.f );
		FillWithLinearData( C, 0.8f,  3.f );
		MemCopy( OUT R.Data(), R.DataSize(), C.Data(), C.DataSize() );

		TEST( GEMV::IsValidDim( A, B, C ));
		GEMV::MulAdd( A, B, INOUT R );

		TestVecMatMulAdd2<float, Layout>( A, B, C, R );
	}


	static void  SimdVecMat_Test2 ()
	{
		const uint		M		= 13 * 4 + 2;
		const uint		K		= 17 * 3;
		const Bytes		align	= 64_b;
		constexpr auto	Layout	= EMatrixLayout::RowMajor;

		LargeVectorStorage< float >				A { K, align };
		LargeMatrixStorage< float, Layout >		B { M, K, align };
		LargeVectorStorage< float >				C { M, align };
		LargeVectorStorage< float >				R { M, align };

		FillWithLinearData( A, 0.1f,  2.f );
		FillWithLinearData( B, 0.4f, -1.f );
		FillWithLinearData( C, 0.8f,  3.f );
		MemCopy( OUT R.Data(), R.DataSize(), C.Data(), C.DataSize() );

		TEST( GEMV::IsValidDim( A, B, C ));
		GEMV::MulAdd( A, B, INOUT R );

		TestVecMatMulAdd2<float, Layout>( A, B, C, R );
	}
}


extern void UnitTest_Math_SimdMatrix ()
{
	SimdMat_Test1();
	SimdMat_Test2();

	SimdVecMat_Test1();
	SimdVecMat_Test2();

	TEST_PASSED();
}
