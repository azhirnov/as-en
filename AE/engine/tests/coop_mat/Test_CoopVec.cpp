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


	void  TestVecMatMul (ArrayView<half> inputA, ArrayView<half> inputB, ArrayView<half> refOutput,
						 const uint rowA, const uint2 rowColB, const uint rowC, const bool columnMajor)
	{
		ASSERT( rowColB.x == rowC );
		ASSERT( rowColB.y == rowA );

		Array<float>	output;
		output.resize( rowC );

		Array<float>	ref_output;
		ref_output.resize( rowC );

		if ( columnMajor )
		{
			for (uint i = 0; i < rowC; ++i)
			{
				float	ref = float(refOutput[i]);
				float	sum = 0.f;

				for (uint j = 0; j < rowA; ++j)
				{
					sum += float(inputB[ i + j * rowC ]) * float(inputA[j]);
				}

				output[i] = sum;
				ref_output[i] = ref;
			}
		}
		else
		{
			for (uint i = 0; i < rowC; ++i)
			{
				float	ref = float(refOutput[i]);
				float	sum = 0.f;

				for (uint j = 0; j < rowA; ++j)
				{
					sum += float(inputB[ i * rowA + j ]) * float(inputA[j]);
				}

				output[i] = sum;
				ref_output[i] = ref;
			}
		}

		String	str		= "\n";
		float	max_err	= 0.f;

		for (uint i = 0; i < rowC; ++i)
		{
			float	err	= Abs( (output[i] - ref_output[i]) * 100.f / output[i] );
			max_err = Max( max_err, err );

			str << "| " << ToString( output[i], 5 ) << " | " << ToString( ref_output[i], 5 ) << " | "
				<< ToString( err, 2 ) << "% |\n";
		}

		str << "----------------------------------------\n\n";
		AE_LOGI( str );
		CHECK( max_err < 1.f );
	}


	void  TestVecMatMulAdd (ArrayView<half> inputA, ArrayView<half> inputB, ArrayView<half> inputC, ArrayView<half> refOutput,
							const uint rowA, const uint2 rowColB, const uint rowC, const bool columnMajor)
	{
		ASSERT( rowColB.x == rowC );
		ASSERT( rowColB.y == rowA );

		Array<float>	output;
		output.resize( rowC );

		Array<float>	ref_output;
		ref_output.resize( rowC );

		if ( columnMajor )
		{
			for (uint i = 0; i < rowC; ++i)
			{
				float	ref = float(refOutput[i]);
				float	sum = float(inputC[i]);

				for (uint j = 0; j < rowA; ++j)
				{
					sum += float(inputB[ i + j * rowC ]) * float(inputA[j]);
				}

				output[i] = sum;
				ref_output[i] = ref;
			}
		}
		else
		{
			for (uint i = 0; i < rowC; ++i)
			{
				float	ref = float(refOutput[i]);
				float	sum = float(inputC[i]);

				for (uint j = 0; j < rowA; ++j)
				{
					sum += float(inputB[ i * rowA + j ]) * float(inputA[j]);
				}

				output[i] = sum;
				ref_output[i] = ref;
			}
		}

		String	str		= "\n";
		float	max_err	= 0.f;

		for (uint i = 0; i < rowC; ++i)
		{
			float	err	= Abs( (output[i] - ref_output[i]) * 100.f / output[i] );
			max_err = Max( max_err, err );

			str << "| " << ToString( output[i], 5 ) << " | " << ToString( ref_output[i], 5 ) << " | "
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


	static void  CoopVec_Test1 (Executor &ex)
	{
		for (uint t = 0; t < 2; ++t)
		{
			const uint		rows_a		= 16;
			const uint		rows_b		= 16;
			const uint		cols_b		= 16;
			const uint		rows_c		= 16;

			StaticAssert( rows_b == rows_c );
			StaticAssert( cols_b == rows_a );

			const uint		vec_count	= 2;
			const bool		col_major	= (t == 0);

			String			src;
			Array<half>		input_a, input_b;
			Array<half>		output;

			input_a.resize( rows_a * vec_count,				half::Zero() );
			input_b.resize( cols_b * rows_b * vec_count,	half::Zero() );
			output .resize( rows_c * vec_count,				half::Zero() );

			FillWithLinearData( input_a, 0.1f,		0.4f );		// vec
			FillWithLinearData( input_b, 0.01f,		-10.f );	// mat

			src << "#define type		half\n"
				<< "#define type_size	" << ToString( sizeof(input_a[0]) ) << '\n'
				<< "#define type_type	gl::ComponentType::Float16\n"
				<< "#define rows_a		" << ToString( rows_a ) << "\n"
				<< "#define rows_b		" << ToString( rows_b ) << "\n"
				<< "#define cols_b		" << ToString( cols_b ) << "\n"
				<< "#define rows_c		" << ToString( rows_c ) << "\n"
				<< "#define vec_count	" << ToString( vec_count ) << "\n"
				<< "#define layout		gl::CoopVectorMatrixLayout::" << (col_major ? "ColumnMajor" : "RowMajor") << '\n'
				<< "#define stride_b	type_size * " << (col_major ? "rows_b" : "cols_b") << '\n';

			src << R"(
				#define				OUT
				#define CoopVecA	gl::CoopVec< type, rows_a >
				#define CoopVecC	gl::CoopVec< type, rows_c >

				void Main ()
				{
					CoopVecA	a;
					CoopVecC	res;

					for (uint i = 0; i < vec_count; ++i)
					{
						uint	first_a		= type_size * i * rows_a;
						uint	first_b		= type_size * i * rows_b * cols_b;
						uint	first_c		= type_size * i * rows_c;

						gl.CoopVecLoad( OUT a, un_InputA.data, first_a );

						gl.CoopVecMatMul(	OUT res,								// output [M]
											a, type_type,							// input [K]
											un_InputB.data, first_b, type_type,		// matrix [M x K]
											rows_b, cols_b,							// M, K
											layout,
											false,									// transpose
											stride_b								// matrixStride
										);

						gl.subgroup.ExecutionBarrier();

						if ( gl.subgroup.Index == 0 )
							gl.CoopVecStore( res, OUT un_Output.data, first_c );
					}
				}
			)";
			CHECK_FATAL( ex.Run( src, BufCast(input_a), BufCast(input_b), BufCast(input_b), BufCast(output) ));

			for (uint i = 0; i < vec_count; ++i)
			{
				auto	in_a	= ArrayView{input_a}.section( i * rows_a,			rows_a );
				auto	in_b	= ArrayView{input_b}.section( i * rows_b * cols_b,	rows_b * cols_b );
				auto	out		= ArrayView{output} .section( i * rows_c,			rows_c );

				TestVecMatMul( in_a, in_b, out, rows_a, uint2{rows_b, cols_b}, rows_c, col_major );
			}
		}
	}


	static void  CoopVec_Test2 (Executor &ex)
	{
		for (uint t = 0; t < 2; ++t)
		{
			const uint		rows_a		= 16;
			const uint		rows_b		= 16;
			const uint		cols_b		= 16;
			const uint		rows_c		= 16;

			StaticAssert( rows_b == rows_c );
			StaticAssert( cols_b == rows_a );

			const uint		vec_count	= 2;
			const bool		col_major	= (t == 0);

			String			src;
			Array<half>		input_a, input_b, input_c;
			Array<half>		output;

			input_a.resize( rows_a * vec_count,				half::Zero() );
			input_b.resize( cols_b * rows_b * vec_count,	half::Zero() );
			input_c.resize( rows_c * vec_count,				half::Zero() );
			output .resize( rows_c * vec_count,				half::Zero() );

			FillWithLinearData( input_a, 0.001f,	1.f );		// vec
			FillWithLinearData( input_b, 0.01f,		0.f );		// mat
			FillWithLinearData( input_c, 0.1f,		2.f );		// vec

			src << "#define type		half\n"
				<< "#define type_size	" << ToString( sizeof(input_a[0]) ) << '\n'
				<< "#define type_type	gl::ComponentType::Float16\n"
				<< "#define rows_a		" << ToString( rows_a ) << "\n"
				<< "#define rows_b		" << ToString( rows_b ) << "\n"
				<< "#define cols_b		" << ToString( cols_b ) << "\n"
				<< "#define rows_c		" << ToString( rows_c ) << "\n"
				<< "#define vec_count	" << ToString( vec_count ) << "\n"
				<< "#define layout		gl::CoopVectorMatrixLayout::" << (col_major ? "ColumnMajor" : "RowMajor") << '\n'
				<< "#define stride_b	type_size * " << (col_major ? "rows_b" : "cols_b") << '\n';

			src << R"(
				#define				OUT
				#define CoopVecA	gl::CoopVec< type, rows_a >
				#define CoopVecC	gl::CoopVec< type, rows_c >

				void Main ()
				{
					CoopVecA	a;
					CoopVecC	res;

					for (uint i = 0; i < vec_count; ++i)
					{
						uint	first_a		= type_size * i * rows_a;
						uint	first_b		= type_size * i * rows_b * cols_b;
						uint	first_c		= type_size * i * rows_c;

						gl.CoopVecLoad( OUT a, un_InputA.data, first_a );

						gl.CoopVecMatMulAdd(	OUT res,								// output [M]
												a, type_type,							// input [K]
												un_InputB.data, first_b, type_type,		// matrix [M x K]
												un_InputC.data, first_c, type_type,		// bias [M]
												rows_b, cols_b,							// M, K
												layout,
												false,									// transpose
												stride_b								// matrixStride
											);

						gl.subgroup.ExecutionBarrier();

						if ( gl.subgroup.Index == 0 )
							gl.CoopVecStore( res, OUT un_Output.data, first_c );
					}
				}
			)";
			CHECK_FATAL( ex.Run( src, BufCast(input_a), BufCast(input_b), BufCast(input_c), BufCast(output) ));

			for (uint i = 0; i < vec_count; ++i)
			{
				auto	in_a	= ArrayView{input_a}.section( i * rows_a,			rows_a );
				auto	in_b	= ArrayView{input_b}.section( i * rows_b * cols_b,	rows_b * cols_b );
				auto	in_c	= ArrayView{input_c}.section( i * rows_c,			rows_c );
				auto	out		= ArrayView{output} .section( i * rows_c,			rows_c );

				TestVecMatMulAdd( in_a, in_b, in_c, out, rows_a, uint2{rows_b, cols_b}, rows_c, col_major );
			}
		}
	}


	static void  CoopVec_Test3 (Executor &ex)
	{
		for (uint t = 0; t < 4; ++t)
		{
			const uint		rows_b		= 16;
			const uint		cols_b		= 16;
			const uint		vec_count	= 1;
			const bool		col_major	= !!(t & 1);
			const bool		inf_opt		= !!(t >> 1);

			Array<half>		input_b, input_b_opt;
			Array<half>		output;

			input_b.resize( cols_b * rows_b * vec_count,	half::Zero() );
			output .resize( cols_b * rows_b * vec_count,	half::Zero() );

			FillWithLinearData( input_b, 0.01f, 0.f );

			BytesUSize		opt_size;
			{
				Graphics::ConvertCoopMatrixOnHost	cmd;

				cmd.srcSize		= ArraySizeOf( input_b ) / vec_count;
				cmd.numRows		= rows_b;
				cmd.numColumns	= cols_b;
				cmd.srcStride	= Bytes{ sizeof(input_b[0]) * rows_b };
				cmd.srcType		= Graphics::ECoopMatrixComponentType::Float16;
				cmd.dstType		= Graphics::ECoopMatrixComponentType::Float16;
				cmd.srcLayout	= col_major ? Graphics::ECoopVecMatrixLayout::ColumnMajor : Graphics::ECoopVecMatrixLayout::RowMajor;
				cmd.dstLayout	= inf_opt ? Graphics::ECoopVecMatrixLayout::InferencingOptimal : Graphics::ECoopVecMatrixLayout::TrainingOptimal;

				CHECK_FATAL( ex.GetDevice().GetCooperativeVectorMatrixDstSize( {cmd}, OUT {opt_size} ));

				ASSERT( IsMultipleOf( opt_size, sizeof(input_b_opt[0]) ));
				input_b_opt.resize( usize{opt_size} / sizeof(input_b_opt[0]) * vec_count );

				for (uint i = 0; i < vec_count; ++i)
				{
					cmd.srcData	= &input_b[ i * cols_b * rows_b ];
					cmd.dstSize	= opt_size;
					cmd.dstData	= input_b_opt.data() + opt_size * i;

					CHECK_FATAL( ex.GetDevice().ConvertCooperativeVectorMatrix( {cmd} ));
				}
			}{
				Graphics::ConvertCoopMatrixOnHost	cmd;
				cmd.srcSize		= opt_size;
				cmd.dstSize		= ArraySizeOf( output ) / vec_count;
				cmd.numRows		= rows_b;
				cmd.numColumns	= cols_b;
				cmd.dstStride	= Bytes{ sizeof(output[0]) * rows_b };
				cmd.srcType		= Graphics::ECoopMatrixComponentType::Float16;
				cmd.dstType		= Graphics::ECoopMatrixComponentType::Float16;
				cmd.srcLayout	= inf_opt ? Graphics::ECoopVecMatrixLayout::InferencingOptimal : Graphics::ECoopVecMatrixLayout::TrainingOptimal;
				cmd.dstLayout	= col_major ? Graphics::ECoopVecMatrixLayout::ColumnMajor : Graphics::ECoopVecMatrixLayout::RowMajor;

				for (uint i = 0; i < vec_count; ++i)
				{
					cmd.srcData		= input_b_opt.data() + opt_size * i;
					cmd.dstData		= &output[ i * cols_b * rows_b ];

					CHECK_FATAL( ex.GetDevice().ConvertCooperativeVectorMatrix( {cmd} ));
				}
			}
			CHECK_FATAL( output == input_b );
		}
	}


	static void  CoopVec_Test4 (Executor &ex)
	{
		for (uint t = 0; t < 4; ++t)
		{
			const uint		rows_a		= 16;
			const uint		rows_b		= 16;
			const uint		cols_b		= 16;
			const uint		rows_c		= 16;

			StaticAssert( rows_b == rows_c );
			StaticAssert( cols_b == rows_a );

			const uint		vec_count	= 1;
			const bool		col_major	= !!(t & 1);
			const bool		inf_opt		= !!(t >> 1);

			String			src;
			Array<half>		input_a, input_b, input_b_opt, input_c;
			Array<half>		output;

			input_a.resize( rows_a * vec_count,				half::Zero() );
			input_b.resize( cols_b * rows_b * vec_count,	half::Zero() );
			input_c.resize( rows_c * vec_count,				half::Zero() );
			output .resize( rows_c * vec_count,				half::Zero() );

			FillWithLinearData( input_a, 0.001f,	1.f );		// vec
			FillWithLinearData( input_b, 0.01f,		0.f );		// mat
			FillWithLinearData( input_c, 0.1f,		2.f );		// vec

			BytesUSize		opt_size;
			{
				Graphics::ConvertCoopMatrixOnHost	cmd;

				cmd.srcSize		= ArraySizeOf( input_b ) / vec_count;
				cmd.numRows		= rows_b;
				cmd.numColumns	= cols_b;
				cmd.srcStride	= Bytes{ sizeof(input_b[0]) * rows_b };
				cmd.srcType		= Graphics::ECoopMatrixComponentType::Float16;
				cmd.dstType		= Graphics::ECoopMatrixComponentType::Float16;
				cmd.srcLayout	= col_major ? Graphics::ECoopVecMatrixLayout::ColumnMajor : Graphics::ECoopVecMatrixLayout::RowMajor;
				cmd.dstLayout	= inf_opt ? Graphics::ECoopVecMatrixLayout::InferencingOptimal : Graphics::ECoopVecMatrixLayout::TrainingOptimal;

				CHECK_FATAL( ex.GetDevice().GetCooperativeVectorMatrixDstSize( {cmd}, {opt_size} ));

				ASSERT( IsMultipleOf( opt_size, sizeof(input_b_opt[0]) ));
				input_b_opt.resize( usize{opt_size} / sizeof(input_b_opt[0]) * vec_count );

				for (uint i = 0; i < vec_count; ++i)
				{
					cmd.srcData	= &input_b[ i * cols_b * rows_b ];
					cmd.dstSize	= opt_size;
					cmd.dstData	= input_b_opt.data() + opt_size * i;

					CHECK_FATAL( ex.GetDevice().ConvertCooperativeVectorMatrix( {cmd} ));
				}
			}

			src << "#define type		half\n"
				<< "#define type_size	" << ToString( sizeof(input_a[0]) ) << '\n'
				<< "#define type_type	gl::ComponentType::Float16\n"
				<< "#define rows_a		" << ToString( rows_a ) << "\n"
				<< "#define rows_b		" << ToString( rows_b ) << "\n"
				<< "#define cols_b		" << ToString( cols_b ) << "\n"
				<< "#define rows_c		" << ToString( rows_c ) << "\n"
				<< "#define vec_count	" << ToString( vec_count ) << "\n"
				<< "#define layout		gl::CoopVectorMatrixLayout::" << (inf_opt ? "InferencingOptimal" : "TrainingOptimal" ) << '\n';

			src << R"(
				#define				OUT
				#define CoopVecA	gl::CoopVec< type, rows_a >
				#define CoopVecC	gl::CoopVec< type, rows_c >

				void Main ()
				{
					CoopVecA	a;
					CoopVecC	res;

					for (uint i = 0; i < vec_count; ++i)
					{
						uint	first_a		= type_size * i * rows_a;
						uint	first_b		= type_size * i * rows_b * cols_b;
						uint	first_c		= type_size * i * rows_c;

						gl.CoopVecLoad( OUT a, un_InputA.data, first_a );

						gl.CoopVecMatMulAdd(	OUT res,								// output [M]
												a, type_type,							// input [K]
												un_InputB.data, first_b, type_type,		// matrix [M x K]
												un_InputC.data, first_c, type_type,		// bias [M]
												rows_b, cols_b,							// M, K
												layout,
												false,									// transpose
												0										// matrixStride, ignored for optimal layout
											);

						gl.subgroup.ExecutionBarrier();

						if ( gl.subgroup.Index == 0 )
							gl.CoopVecStore( res, OUT un_Output.data, first_c );
					}
				}
			)";
			CHECK_FATAL( ex.Run( src, BufCast(input_a), BufCast(input_b_opt), BufCast(input_c), BufCast(output) ));

			for (uint i = 0; i < vec_count; ++i)
			{
				auto	in_a	= ArrayView{input_a}.section( i * rows_a,			rows_a );
				auto	in_b	= ArrayView{input_b}.section( i * rows_b * cols_b,	rows_b * cols_b );
				auto	in_c	= ArrayView{input_c}.section( i * rows_c,			rows_c );
				auto	out		= ArrayView{output} .section( i * rows_c,			rows_c );

				TestVecMatMulAdd( in_a, in_b, in_c, out, rows_a, uint2{rows_b, cols_b}, rows_c, col_major );
			}
		}
	}

} // namespace


extern void Test_CoopVec (Executor &ex)
{
	if ( not ex.SupportsCoopVector() )
		return;

	CoopVec_Test1( ex );
	CoopVec_Test2( ex );
	CoopVec_Test3( ex );
	CoopVec_Test4( ex );

	TEST_PASSED();
}
