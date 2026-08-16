// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Executor.h"

namespace
{
	using ByteBuffer = Executor::ByteBuffer;

	template <typename T>
	ND_ ByteBuffer  BufCast (Array<T> &arr)
	{
		return ByteBuffer{ Cast<ubyte>(arr.data()), arr.size() * sizeof(T) };
	}


	void  TestVecReduceSumAccum (ArrayView<half> inputA, ArrayView<half> inputB, ArrayView<half> refOutput, const uint rowA)
	{
		ASSERT( inputA.size() == rowA );
		ASSERT( inputA.size() == inputB.size() );
		ASSERT( inputA.size() == refOutput.size() );

		Array<float>	ref_output;
		ref_output.resize( rowA );

		Array<float>	output;
		output.resize( rowA );

		for (usize i = 0; i < rowA; ++i)
		{
			float	a	= float(inputA[i]);
			float	b	= float(inputB[i]);

			output[i] = a + b;
			ref_output[i] = float(refOutput[i]);
		}

		String	str		= "\n| expected | shader output | error % |\n";
		float	max_err	= 0.f;
		float	avr_err	= 0.f;

		for (usize i = 0; i < rowA; ++i)
		{
			float	err	= Abs(output[i] - ref_output[i]) * 100.f / Max( Abs(output[i]), 1.0e-5f );
			max_err = Max( max_err, err );
			avr_err += err;

			str << "| " << ToString( output[i], 5 ) << " | " << ToString( ref_output[i], 5 ) << " | "
				<< ToString( err, 2 ) << "% |\n";
		}
		avr_err /= float(rowA);
		str << "----------------------------------------\n";

		if ( max_err < 0.01f )
			str.clear();

		str << "max error: " << ToString( max_err, 2 ) << "%, avr: " << ToString( avr_err, 2 ) << "%";
		AE_LOGI( str );
		CHECK( max_err < 1.f );
	}


	void  TestVecOuterProductAccum (ArrayView<half> inputA, ArrayView<half> inputB, ArrayView<half> inputC, ArrayView<half> refOutput,
									const uint rowA, const uint rowB, const bool columnMajor, const uint subgroupSize)
	{
		ASSERT( inputC.size() == refOutput.size() );
		ASSERT( inputC.size() == rowA * rowB );

		Array<float>	ref_output;
		ref_output.resize( refOutput.size() );

		Array<float>	output;
		output.resize( refOutput.size() );

		for (usize i = 0; i < inputC.size(); ++i)
		{
			output[i] = float(inputC[i]);
		}

		if ( columnMajor )
		{
			for (uint col = 0; col < rowB; ++col)
			{
				for (uint row = 0; row < rowA; ++row)
				{
					uint	idx	= row + col * rowB;
					float	ref	= float(refOutput[ idx ]);
					float	a	= float(inputA[row]);
					float	b	= float(inputB[col]);

					output[idx] += a * b * float(subgroupSize);
					ref_output[idx] = ref;
				}
			}
		}
		else
		{
			for (uint row = 0; row < rowB; ++row)
			{
				for (uint col = 0; col < rowA; ++col)
				{
					uint	idx	= row * rowA + col;
					float	ref	= float(refOutput[ idx ]);
					float	a	= float(inputA[row]);
					float	b	= float(inputB[col]);

					output[idx] += a * b * float(subgroupSize);
					ref_output[idx] = ref;
				}
			}
		}

		String	str		= "\n| expected | shader output | error % |\n";
		float	max_err	= 0.f;
		float	avr_err	= 0.f;

		for (usize i = 0; i < refOutput.size(); ++i)
		{
			float	err	= Abs(output[i] - ref_output[i]) * 100.f / Max( Abs(output[i]), 1.0e-5f );
			max_err = Max( max_err, err );
			avr_err += err;

			str << "| " << ToString( output[i], 5 ) << " | " << ToString( ref_output[i], 5 ) << " | "
				<< ToString( err, 2 ) << "% |\n";
		}
		avr_err /= float(refOutput.size());
		str << "----------------------------------------\n";

		if ( max_err < 0.01f )
			str.clear();

		str << "max error: " << ToString( max_err, 2 ) << "%, avr: " << ToString( avr_err, 2 ) << "%";
		AE_LOGI( str );
		CHECK( max_err < 1.f );
	}


	void  FillWithLinearData (INOUT Array<half> &arr, float scale, float bias)
	{
		for (usize i = 0; i < arr.size(); ++i) {
			arr[i] = half( float(i) * scale + bias );
		}
	}


	static void  CoopVecTraining_Test1 (Executor &ex)
	{
		CHECK_FATAL( ex.GetDevice().GetVProperties().cooperativeVectorNVProps.cooperativeVectorTrainingFloat16Accumulation == VK_TRUE );

		const uint		rows_a		= 16;
		const uint		vec_count	= 2;

		String			src;
		Array<half>		input_a, input_b;
		Array<half>		output;

		input_a.resize( rows_a * vec_count,	half::Zero() );
		input_b.resize( rows_a * vec_count,	half::Zero() );

		FillWithLinearData( input_a, 0.1f,	1.f );		// vec
		FillWithLinearData( input_b, 0.2f,	-10.f );	// vec
		output = input_b;

		src << "#define type		half\n"
			<< "#define type_size	" << ToString( sizeof(input_a[0]) ) << '\n'
			<< "#define type_type	gl::ComponentType::Float16\n"
			<< "#define rows_a		" << ToString( rows_a ) << "\n"
			<< "#define vec_count	" << ToString( vec_count ) << "\n";

		src << R"(
			#define				OUT
			#define				INOUT
			#define CoopVec		gl::CoopVec< type, rows_a >

			void Main ()
			{
				CoopVec	a;

				for (uint i = 0; i < vec_count; ++i)
				{
					uint	first_a	= type_size * i * rows_a;

					gl.CoopVecLoad( OUT a, un_InputA.data, first_a );

					if ( gl.subgroup.Index == 0 )
					{
						gl.CoopVecReduceSumAccum( a,
												  INOUT un_Output.data,
												  first_a );
					}
				}
			}
		)";
		CHECK_FATAL( ex.Run( src, BufCast(input_a), BufCast(input_b), BufCast(input_b), BufCast(output), sizeof(half) ));

		for (uint i = 0; i < vec_count; ++i)
		{
			auto	in_a	= ArrayView{input_a}.section( i * rows_a,	rows_a );
			auto	in_b	= ArrayView{input_b}.section( i * rows_a,	rows_a );
			auto	out		= ArrayView{output} .section( i * rows_a,	rows_a );

			TestVecReduceSumAccum( in_a, in_b, out, rows_a );
		}
	}


	static void  CoopVecTraining_Test2 (Executor &ex)
	{
		CHECK_FATAL( ex.GetDevice().GetVProperties().cooperativeVectorNVProps.cooperativeVectorTrainingFloat16Accumulation == VK_TRUE );

		const uint	subgroup_size = ex.GetDevice().GetDeviceProperties().compute.subgroupSize;

		for (uint t = 0; t < 2; ++t)
		{
			const uint		rows_a		= 16;
			const uint		rows_b		= 16;
			const uint		rows_c		= rows_a;
			const uint		cols_c		= rows_b;

			const uint		vec_count	= 1;	// TODO
			const bool		col_major	= (t == 0);

			String			src;
			Array<half>		input_a, input_b, input_c;
			Array<half>		output, output_opt;

			input_a.resize( rows_a * vec_count,				half::Zero() );
			input_b.resize( rows_b * vec_count,				half::Zero() );
			input_c.resize( rows_c * cols_c * vec_count,	half::Zero() );

			FillWithLinearData( input_a, 0.1f,		1.1f );		// vec
			FillWithLinearData( input_b, 0.2f,		-10.1f );	// vec
			FillWithLinearData( input_c, 0.01f,		-2.f );		// mat
			output = input_c;

			// 'output' -> 'output_opt'
			BytesUSize		opt_size;
			{
				Graphics::ConvertCoopMatrixOnHost	cmd;

				cmd.srcSize		= ArraySizeOf( output ) / vec_count;
				cmd.numRows		= rows_c;
				cmd.numColumns	= cols_c;
				cmd.srcStride	= Bytes{ sizeof(output[0]) * rows_c };
				cmd.srcType		= Graphics::ECoopMatrixComponentType::Float16;
				cmd.dstType		= Graphics::ECoopMatrixComponentType::Float16;
				cmd.srcLayout	= col_major ? Graphics::ECoopVecMatrixLayout::ColumnMajor : Graphics::ECoopVecMatrixLayout::RowMajor;
				cmd.dstLayout	= Graphics::ECoopVecMatrixLayout::TrainingOptimal;

				CHECK_FATAL( ex.GetDevice().GetCooperativeVectorMatrixDstSize( {cmd}, OUT {opt_size} ));
				AE_LOGI( "CoopVec opt size: "s << ToString(opt_size) );

				ASSERT( IsMultipleOf( opt_size, sizeof(output_opt[0]) ));
				output_opt.resize( usize{opt_size} / sizeof(output_opt[0]) * vec_count );

				for (uint i = 0; i < vec_count; ++i)
				{
					cmd.srcData	= output.data() + cmd.srcSize * i;
					cmd.dstSize	= opt_size;
					cmd.dstData	= output_opt.data() + opt_size * i;

					CHECK_FATAL( ex.GetDevice().ConvertCooperativeVectorMatrix( {cmd} ));
				}
			}

			src << "#define type		half\n"
				<< "#define type_size	" << ToString( sizeof(input_a[0]) ) << '\n'
				<< "#define type_type	gl::ComponentType::Float16\n"
				<< "#define rows_a		" << ToString( rows_a ) << "\n"
				<< "#define rows_b		" << ToString( rows_b ) << "\n"
				<< "#define vec_count	" << ToString( vec_count ) << "\n"
				<< "#define stride_c	" << ToString( usize{opt_size} ) << '\n';

			src << R"(
				#define				OUT
				#define				INOUT
				#define CoopVecA	gl::CoopVec< type, rows_a >
				#define CoopVecB	gl::CoopVec< type, rows_b >

				void Main ()
				{
					CoopVecA	a;
					CoopVecB	b;

					for (uint i = 0; i < vec_count; ++i)
					{
						uint	first_a		= type_size * i * rows_a;
						uint	first_b		= type_size * i * rows_b;
						uint	first_c		= type_size * i * stride_c;

						gl.CoopVecLoad( OUT a, un_InputA.data, first_a );
						gl.CoopVecLoad( OUT b, un_InputB.data, first_b );

						//if ( gl.subgroup.Index == 0 )	// otherwise result is incorrect
						{
							gl.CoopVecOuterProductAccum( a, b,
														 INOUT un_Output.data,
														 first_c,
														 0, // ignored
														 gl::CoopVectorMatrixLayout::TrainingOptimal,
														 type_type
														);
						}
					}
				}
			)";
			CHECK_FATAL( ex.Run( src, BufCast(input_a), BufCast(input_b), BufCast(input_c), BufCast(output_opt), sizeof(half) ));

			// 'output_opt' -> 'output'
			{
				Graphics::ConvertCoopMatrixOnHost	cmd;
				cmd.srcSize		= opt_size;
				cmd.dstSize		= ArraySizeOf( output ) / vec_count;
				cmd.numRows		= rows_c;
				cmd.numColumns	= cols_c;
				cmd.dstStride	= Bytes{ sizeof(output[0]) * rows_c };
				cmd.srcType		= Graphics::ECoopMatrixComponentType::Float16;
				cmd.dstType		= Graphics::ECoopMatrixComponentType::Float16;
				cmd.srcLayout	= Graphics::ECoopVecMatrixLayout::TrainingOptimal;
				cmd.dstLayout	= col_major ? Graphics::ECoopVecMatrixLayout::ColumnMajor : Graphics::ECoopVecMatrixLayout::RowMajor;

				for (uint i = 0; i < vec_count; ++i)
				{
					cmd.srcData		= output_opt.data() + opt_size * i;
					cmd.dstData		= output.data() + cmd.dstSize * i;

					CHECK_FATAL( ex.GetDevice().ConvertCooperativeVectorMatrix( {cmd} ));
				}
			}

			for (uint i = 0; i < vec_count; ++i)
			{
				auto	in_a	= ArrayView{input_a}.section( i * rows_a,			rows_a );
				auto	in_b	= ArrayView{input_b}.section( i * rows_b,			rows_b );
				auto	in_c	= ArrayView{input_c}.section( i * rows_c * cols_c,	rows_c * cols_c );
				auto	out		= ArrayView{output} .section( i * rows_c * cols_c,	rows_c * cols_c );

				TestVecOuterProductAccum( in_a, in_b, in_c, out, rows_a, rows_c, col_major, subgroup_size );
			}
		}
	}

} // namespace


extern void Test_CoopVecTraining (Executor &ex)
{
	if ( not ex.SupportsCoopVecTraining() )
		return;

	CoopVecTraining_Test1( ex );
	CoopVecTraining_Test2( ex );

	TEST_PASSED();
}
