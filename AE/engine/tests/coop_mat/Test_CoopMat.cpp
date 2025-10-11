// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'

#include "Executor.h"

namespace
{
	using ByteBuffer = Executor::ByteBuffer;

	template <typename T>
	ND_ ByteBuffer  BufCast (Array<T> &arr)
	{
		return ByteBuffer{ Cast<ubyte>(arr.data()), arr.size() * sizeof(T) };
	}


	void  TestMatMulAdd (ArrayView<half> inputA, ArrayView<half> inputB, ArrayView<half> inputC, ArrayView<half> refOutput,
						 const uint2 rowColA, const uint2 rowColB, const uint2 rowColC, const bool columnMajor)
	{
		ASSERT( inputA.size() == rowColA.x * rowColA.y );
		ASSERT( inputB.size() == rowColB.x * rowColB.y );
		ASSERT( inputC.size() == rowColC.x * rowColC.y );

		ASSERT( rowColA.y == rowColB.x );
		ASSERT( rowColA.x == rowColC.x );
		ASSERT( rowColB.y == rowColC.y );

		Array<float>	output;
		output.resize( rowColC.x * rowColC.y );
		
		Array<float>	ref_output;
		ref_output.resize( rowColC.x * rowColC.y );

		if ( columnMajor )
		{
			for (uint i = 0; i < rowColA.x; ++i)
			{
				for (uint j = 0; j < rowColB.y; ++j)
				{
					const uint	idx  = i * rowColB.y + j;
					float		sum  = float(inputC[ idx ]);
					float		ref  = float(refOutput[ idx ]);

					for (uint k = 0; k < rowColA.y; ++k)
					{
						float	a = float(inputA[ k * rowColA.y + j ]);		// col
						float	b = float(inputB[ i * rowColB.y + k ]);		// row

						sum += a * b;
					}

					output[ idx ] = sum;
					ref_output[ idx ] = ref;
				}
			}
		}
		else // row-major
		{
			for (uint i = 0; i < rowColA.x; ++i)
			{
				for (uint j = 0; j < rowColB.y; ++j)
				{
					const uint	idx  = i * rowColB.y + j;
					float		sum  = float(inputC[ idx ]);
					float		ref  = float(refOutput[ idx ]);

					for (uint k = 0; k < rowColA.y; ++k)
					{
						float	a = float(inputA[ i * rowColA.y + k ]);		// row
						float	b = float(inputB[ k * rowColB.y + j ]);		// col

						sum += a * b;
					}

					output[ idx ] = sum;
					ref_output[ idx ] = ref;
				}
			}
		}

		String	str		= "\n";
		float	max_err	= 0.f;

		for (uint i = 0; i < rowColC.x; ++i)
		for (uint j = 0; j < rowColC.y; ++j)
		{
			uint	idx	= i * rowColC.y + j;
			float	err	= Abs( (output[idx] - ref_output[idx]) * 100.f / output[idx] );

			max_err = Max( max_err, err );

			str << "| " << ToString( output[idx], 5 ) << " | " << ToString( ref_output[idx], 5 ) << " | "
				<< ToString( err, 2 ) << "% |\n";
		}

		str << "----------------------------------------\n\n";
		AE_LOGI( str );
		CHECK( max_err < 1.f );
	}

	
	void  FillWithLinearData (INOUT Array<half> &arr, float scale, float bias)
	{
		for (usize i = 0; i < arr.size(); ++i) {
			arr[i] = half( float(i) * scale + bias );
		}
	}


	static void  CoopMat_Test1 (Executor &ex)
	{
		for (uint t = 0; t < 2; ++t)
		{
			const uint		rows_a		= 16;
			const uint		cols_a		= 16;
			const uint		rows_b		= 16;
			const uint		cols_b		= 16;
			const uint		rows_c		= 16;
			const uint		cols_c		= 16;

			const uint		mat_count	= 2;
			const bool		col_major	= (t == 0);
		
			StaticAssert( cols_a == rows_b );
			StaticAssert( rows_a == rows_c );
			StaticAssert( cols_b == cols_c );

			String			src;
			Array<half>		input_a, input_b, input_c;
			Array<half>		output;

			input_a.resize( cols_a * rows_a * mat_count, half::Zero() );
			input_b.resize( cols_b * rows_b * mat_count, half::Zero() );
			input_c.resize( cols_c * rows_c * mat_count, half::Zero() );
			output .resize( cols_c * rows_c * mat_count, half::Zero() );

			FillWithLinearData( input_a, 0.001f,	1.f );
			FillWithLinearData( input_b, 0.01f,		-10.f );
			FillWithLinearData( input_c, 0.02f,		0.1f );

			src << "#define type		half\n"
				<< "#define rows_a		" << ToString( rows_a ) << '\n'
				<< "#define cols_a		" << ToString( cols_a ) << '\n'
				<< "#define rows_b		" << ToString( rows_b ) << '\n'
				<< "#define cols_b		" << ToString( cols_b ) << '\n'
				<< "#define rows_c		" << ToString( rows_c ) << '\n'
				<< "#define cols_c		" << ToString( cols_c ) << '\n'
				<< "#define mat_count	" << ToString( mat_count ) << '\n'
				<< "#define layout		gl::CooperativeMatrixLayout::" << (col_major ? "ColumnMajor" : "RowMajor") << '\n'
				<< "#define stride_a	" << (col_major ? "rows_a" : "cols_a") << '\n'
				<< "#define stride_b	" << (col_major ? "rows_b" : "cols_b") << '\n'
				<< "#define stride_c	" << (col_major ? "rows_c" : "cols_c") << '\n';

			src << R"(
				#define				OUT
				#define CoopMatA	gl::CoopMat< type, gl::Scope::Subgroup, rows_a, cols_a, gl::MatrixUse::A >
				#define CoopMatB	gl::CoopMat< type, gl::Scope::Subgroup, rows_b, cols_b, gl::MatrixUse::B >
				#define CoopMatC	gl::CoopMat< type, gl::Scope::Subgroup, rows_c, cols_c, gl::MatrixUse::C >

				void Main ()
				{
					CoopMatA	a;
					CoopMatB	b;
					CoopMatC	c, res;

					for (uint i = 0; i < mat_count; ++i)
					{
						uint	first_a		= i * rows_a * cols_a;
						uint	first_b		= i * rows_b * cols_b;
						uint	first_c		= i * rows_c * cols_c;

						gl.CoopMatLoad( OUT a, un_InputA.data, first_a, stride_a, layout );
						gl.CoopMatLoad( OUT b, un_InputB.data, first_b, stride_b, layout );
						gl.CoopMatLoad( OUT c, un_InputC.data, first_c, stride_c, layout );

						res = gl.CoopMatMulAdd( a, b, c, 0 );	// TODO: different operands

						// whole subgroup must store data
						gl.CoopMatStore( res, OUT un_Output.data, first_c, stride_c, layout );
					}
				}
			)";
			CHECK_FATAL( ex.Run( src, BufCast(input_a), BufCast(input_b), BufCast(input_c), BufCast(output) ));

			for (uint i = 0; i < mat_count; ++i)
			{
				auto	in_a	= ArrayView{input_a}.section( i * rows_a * cols_a, rows_a * cols_a );
				auto	in_b	= ArrayView{input_b}.section( i * rows_b * cols_b, rows_b * cols_b );
				auto	in_c	= ArrayView{input_c}.section( i * rows_c * cols_c, rows_c * cols_c );
				auto	out		= ArrayView{output} .section( i * rows_c * cols_c, rows_c * cols_c );

				TestMatMulAdd( in_a, in_b, in_c, out, uint2{rows_a, cols_a}, uint2{rows_b, cols_b}, uint2{rows_c, cols_c}, col_major );
			}
		}
	}
}


extern void Test_CoopMat (Executor &ex)
{
	if ( not ex.SupportsCoopMatrix() )
		return;

	CoopMat_Test1( ex );

	TEST_PASSED();
}
