// Copyright (c) Zhirnov Andrey. For more information see 'AE/LICENSE.md'

#include "Executor.h"
#include "graphics_rhi/Private/EnumToString.h"

namespace
{
	using ByteBuffer = Executor::ByteBuffer;

	template <typename T>
	ND_ ByteBuffer  BufCast (Array<T> &arr)
	{
		return ByteBuffer{ Cast<ubyte>(arr.data()), arr.size() * sizeof(T) };
	}


	template <typename A, typename B, typename C>
	void  TestMatMulAdd (ArrayView<A> inputA, ArrayView<B> inputB, ArrayView<C> inputC, ArrayView<C> refOutput,
						 const MatrixDim dimA, const MatrixDim dimB, const MatrixDim dimC, const bool columnMajor)
	{
		ASSERT( inputA.size() == dimA.rows * dimA.columns );
		ASSERT( inputB.size() == dimB.rows * dimB.columns );
		ASSERT( inputC.size() == dimC.rows * dimC.columns );

		ASSERT( dimA.columns == dimB.rows );
		ASSERT( dimA.rows	 == dimC.rows );
		ASSERT( dimB.columns == dimC.columns );

		Array<float>	output;
		output.resize( dimC.Size() );

		Array<float>	ref_output;
		ref_output.resize( dimC.Size() );

		if ( columnMajor )
		{
			for (uint i = 0; i < dimA.rows; ++i)
			{
				for (uint j = 0; j < dimB.columns; ++j)
				{
					const uint	idx  = j * dimC.rows + i;
					float		sum  = float(inputC[ idx ]);
					float		ref  = float(refOutput[ idx ]);

					for (uint k = 0; k < dimA.columns; ++k)
					{
						float	a = float(inputA[ k * dimA.rows + i ]);		// col
						float	b = float(inputB[ j * dimB.rows + k ]);		// row

						sum += a * b;
					}

					output[ idx ] = sum;
					ref_output[ idx ] = ref;
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
					float		ref  = float(refOutput[ idx ]);

					for (uint k = 0; k < dimA.columns; ++k)
					{
						float	a = float(inputA[ i * dimA.columns + k ]);		// row
						float	b = float(inputB[ k * dimB.columns + j ]);		// col

						sum += a * b;
					}

					output[ idx ] = sum;
					ref_output[ idx ] = ref;
				}
			}
		}

		String	str		= "\n| C,R | expected | shader output | error % |\n";
		float	max_err	= 0.f;
		float	avr_err	= 0.f;

		for (uint i = 0; i < dimC.rows; ++i)
		for (uint j = 0; j < dimC.columns; ++j)
		{
			uint	idx = columnMajor ?
							j * dimC.rows + i :
							i * dimC.columns + j;

			float	err	= Abs(output[idx] - ref_output[idx]) * 100.f / Max( Abs(output[idx]), 1.0e-5f );
			max_err = Max( max_err, err );
			avr_err += err;

			str << "| " << ToString( i ) << ", " << ToString( j ) << " | "
				<< ToString( output[idx], 5 ) << " | " << ToString( ref_output[idx], 5 ) << " | "
				<< ToString( err, 2 ) << "% |\n";
		}
		avr_err /= float(dimC.Size());
		str << "----------------------------------------\n";

		if ( max_err < 0.01f )
			str.clear();

		str << "max error: " << ToString( max_err, 2 ) << "%, avr: " << ToString( avr_err, 2 ) << "%";
		AE_LOGI( str );
		CHECK( max_err < 1.f );
	}


	template <typename T>
	void  FillWithLinearData (INOUT Array<T> &arr, float scale, float bias)
	{
		for (usize i = 0; i < arr.size(); ++i) {
			arr[i] = T( float(i) * scale + bias );
		}
	}


	template <typename CType>
	static void  CoopMat_Test1Impl (Executor &ex, const Graphics::CoopMatrixConfig &cfg)
	{
		const uint	rows_a	= cfg.m;
		const uint	cols_a	= cfg.k;
		const uint	rows_b	= cfg.k;
		const uint	cols_b	= cfg.n;
		const uint	rows_c	= cfg.m;
		const uint	cols_c	= cfg.n;

		// A, B, C, R:		R = A * B + C
		// M, N, K:			A[MxK], B[KxN], C[MxN], R[MxN]
		CHECK( cols_a == rows_b );	// K
		CHECK( rows_a == rows_c );	// M
		CHECK( cols_b == cols_c );	// N

		for (uint t = 0; t < 2; ++t)
		{
			const uint		mat_count	= 2;
			const bool		col_major	= (t == 0);

			String			src;
			Array<half>		input_a, input_b;
			Array<CType>	input_c, output;

			input_a.resize( cols_a * rows_a * mat_count, half::Zero() );
			input_b.resize( cols_b * rows_b * mat_count, half::Zero() );
			input_c.resize( cols_c * rows_c * mat_count, CType(0.f) );
			output .resize( cols_c * rows_c * mat_count, CType(0.f) );

			FillWithLinearData( input_a, 0.001f,	1.f );
			FillWithLinearData( input_b, 0.01f,		-10.f );
			FillWithLinearData( input_c, 0.02f,		0.1f );

			src << "#define type_ab		half\n"
				<< "#define type_c		" << (cfg.c == Graphics::ECoopMatrixComponentType::Float32 ? "float" : "half") << '\n'
				<< "#define c_scale		" << (cfg.c == Graphics::ECoopMatrixComponentType::Float32 ? "2" : "1") << '\n'	// because 'ex.Run()' gets 'sizeof(half)' - 2bytes
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
				<< "#define stride_c	(" << (col_major ? "rows_c" : "cols_c") << " * c_scale)\n";

			src << R"(
				#define				OUT
				#define CoopMatA	gl::CoopMat< type_ab, gl::Scope::Subgroup, rows_a, cols_a, gl::MatrixUse::A >
				#define CoopMatB	gl::CoopMat< type_ab, gl::Scope::Subgroup, rows_b, cols_b, gl::MatrixUse::B >
				#define CoopMatC	gl::CoopMat< type_c,  gl::Scope::Subgroup, rows_c, cols_c, gl::MatrixUse::C >

				void Main ()
				{
					CoopMatA	a;
					CoopMatB	b;
					CoopMatC	c, res;

					for (uint i = 0; i < mat_count; ++i)
					{
						uint	first_a		= i * rows_a * cols_a;
						uint	first_b		= i * rows_b * cols_b;
						uint	first_c		= i * rows_c * cols_c * c_scale;

						gl.CoopMatLoad( OUT a, un_InputA.data, first_a, stride_a, layout );
						gl.CoopMatLoad( OUT b, un_InputB.data, first_b, stride_b, layout );
						gl.CoopMatLoad( OUT c, un_InputC.data, first_c, stride_c, layout );

						res = gl.CoopMatMulAdd( a, b, c, 0 );

						// whole subgroup must store data
						gl.CoopMatStore( res, OUT un_Output.data, first_c, stride_c, layout );
					}
				}
			)";
			CHECK_FATAL( ex.Run( src, BufCast(input_a), BufCast(input_b), BufCast(input_c), BufCast(output), sizeof(half) ));

			for (uint i = 0; i < mat_count; ++i)
			{
				auto	in_a	= ArrayView{input_a}.section( i * rows_a * cols_a, rows_a * cols_a );
				auto	in_b	= ArrayView{input_b}.section( i * rows_b * cols_b, rows_b * cols_b );
				auto	in_c	= ArrayView{input_c}.section( i * rows_c * cols_c, rows_c * cols_c );
				auto	out		= ArrayView{output} .section( i * rows_c * cols_c, rows_c * cols_c );

				TestMatMulAdd( in_a, in_b, in_c, out, MatrixDim{cols_a, rows_a}, MatrixDim{cols_b, rows_b}, MatrixDim{cols_c, rows_c}, col_major );
			}
		}
	}


	static void  CoopMat_Test1 (Executor &ex)
	{
		using namespace AE::Graphics;

		auto	mat_bits = GraphicsScheduler().GetFeatureSet().cooperativeMatrixConfig;
		for (auto e : mat_bits)
		{
			CoopMatrixConfig	cfg{e};

			if ( cfg.a != ECoopMatrixComponentType::Float16 or
				 cfg.b != ECoopMatrixComponentType::Float16 or
				 cfg.c != cfg.res )
				continue;

			AE_LOGI( "Test config: "s << ToString(e) );

			switch ( cfg.c )
			{
				case ECoopMatrixComponentType::Float16 :
					CoopMat_Test1Impl<half>( ex, cfg );
					break;

				case ECoopMatrixComponentType::Float32 :
					CoopMat_Test1Impl<float>( ex, cfg );
					break;
			}
		}
	}


	template <typename CType>
	static void  CoopMat_Test2Impl (Executor &ex, const Graphics::CoopMatrixConfig &cfg, const uint M, const uint N, const uint K)
	{
		const uint	rows_a	= cfg.m;
		const uint	cols_a	= cfg.k;
		const uint	rows_b	= cfg.k;
		const uint	cols_b	= cfg.n;
		const uint	rows_c	= cfg.m;
		const uint	cols_c	= cfg.n;

		// A, B, C, R:		R = A * B + C
		// M, N, K:			A[MxK], B[KxN], C[MxN], R[MxN]
		CHECK( cols_a == rows_b );	// K tile
		CHECK( rows_a == rows_c );	// M tile
		CHECK( cols_b == cols_c );	// N tile

		// large matrix which divided on tiles
		const uint	num_tiles_m	= DivCeil( M, cfg.m );
		const uint	num_tiles_n	= DivCeil( N, cfg.n );
		const uint	sg_tiles_m	= IsMultipleOf( num_tiles_m, 2 ) ? 2 : 1;
		const uint	sg_tiles_n	= IsMultipleOf( num_tiles_n, 2 ) ? 2 : 1;

		Executor::WGConfig	wg_cfg;
		wg_cfg.subgroupCount	= sg_tiles_m * sg_tiles_n;	// 1..4
		wg_cfg.wgCount.x		= num_tiles_m / sg_tiles_m;
		wg_cfg.wgCount.y		= num_tiles_n / sg_tiles_n;

		// to avoid out-of-bounds read
		CHECK( IsMultipleOf( M, cfg.m ));
		CHECK( IsMultipleOf( N, cfg.n ));
		CHECK( IsMultipleOf( K, cfg.k ));

		const bool	is_fp32 = cfg.c == Graphics::ECoopMatrixComponentType::Float32;
		CHECK( sizeof(CType) == (is_fp32 ? 4 : 2) );

		for (uint t = 0; t < 2; ++t)
		{
			const bool		col_major	= t > 0;

			String			src;
			Array<half>		input_a, input_b;
			Array<CType>	input_c, output;

			input_a.resize( M * K, half::Zero() );
			input_b.resize( K * N, half::Zero() );
			input_c.resize( M * N, CType(0.f) );
			output .resize( M * N, CType(0.f) );

			FillWithLinearData( input_c, 0.02f,	0.1f );

			src << "#define type_c		" << (is_fp32 ? "float" : "half") << '\n'
				<< "#define c_scale		" << (is_fp32 ? "2" : "1") << '\n'		// because 'ex.Run()' gets 'sizeof(half)' - 2bytes
				<< "#define rows_a		" << ToString( rows_a ) << '\n'
				<< "#define cols_b		" << ToString( cols_b ) << '\n'
				<< "#define rows_c		" << ToString( rows_c ) << '\n'
				<< "#define cols_c		" << ToString( cols_c ) << '\n'
				<< "#define layout		gl::CooperativeMatrixLayout::" << (col_major ? "ColumnMajor" : "RowMajor") << '\n'
				<< "#define COL_MAJOR   " << ToString( col_major ? "1" : "0") << '\n'
				<< "#define stride_c    " << ToString( (col_major ? M : N) * (is_fp32 ? 2 : 1) ) << '\n'
				<< "#define mat_m       " << ToString( M ) << '\n'
				<< "#define mat_n       " << ToString( N ) << '\n'
				<< "#define mat_k       " << ToString( K ) << '\n'
				<< "#define sg_tiles_n	" << ToString( sg_tiles_n ) << '\n'
				<< "#define sg_tiles_m	" << ToString( sg_tiles_m ) << '\n';

			src << R"(
				#define				OUT
				#define CoopMatC	gl::CoopMat< type_c,  gl::Scope::Subgroup, rows_c, cols_c, gl::MatrixUse::C >

				void Main ()
				{
					CoopMatC	c;

					uint	tile_n		= gl.subgroup.GroupIndex % sg_tiles_n + gl.WorkGroupID.y * sg_tiles_n;
					uint	tile_m		= gl.subgroup.GroupIndex / sg_tiles_n + gl.WorkGroupID.x * sg_tiles_m;

					uint	offset_n	= cols_b * tile_n;
					uint	offset_m	= rows_a * tile_m;

					// in elements
				  #if COL_MAJOR
					uint	first_c		= (offset_n * mat_m + offset_m) * c_scale;
				  #else
					uint	first_c		= (offset_m * mat_n + offset_n) * c_scale;
				  #endif

					gl.CoopMatLoad( OUT c, un_InputC.data, first_c, stride_c, layout );

					// whole subgroup must store data
					gl.CoopMatStore( c, OUT un_Output.data, first_c, stride_c, layout );
				}
			)";
			CHECK_FATAL( ex.Run( src, BufCast(input_a), BufCast(input_b), BufCast(input_c), BufCast(output), sizeof(half), wg_cfg ));


			TestMatMulAdd<half, half, CType>( input_a, input_b, input_c, output, MatrixDim{K, M}, MatrixDim{N, K}, MatrixDim{N, M}, col_major );
		}
	}


	static void  CoopMat_Test2 (Executor &ex)
	{
		using namespace AE::Graphics;

		const uint	M = 64;
		const uint	N = M;
		const uint	K = M;

		auto	mat_bits = GraphicsScheduler().GetFeatureSet().cooperativeMatrixConfig;
		for (auto e : mat_bits)
		{
			CoopMatrixConfig	cfg{e};

			if ( cfg.a != ECoopMatrixComponentType::Float16 or
				 cfg.b != ECoopMatrixComponentType::Float16 or
				 cfg.c != cfg.res )
				continue;

			AE_LOGI( "Test config: "s << ToString(e) );

			switch ( cfg.c )
			{
				case ECoopMatrixComponentType::Float16 :
					CoopMat_Test2Impl<half>( ex, cfg, M, N, K );
					break;

				case ECoopMatrixComponentType::Float32 :
					CoopMat_Test2Impl<float>( ex, cfg, M, N, K );
					break;
			}
		}
	}


	template <typename CType>
	static void  CoopMat_Test3Impl (Executor &ex, const Graphics::CoopMatrixConfig &cfg, const uint M, const uint N, const uint K)
	{
		const uint	rows_a	= cfg.m;
		const uint	cols_a	= cfg.k;
		const uint	rows_b	= cfg.k;
		const uint	cols_b	= cfg.n;
		const uint	rows_c	= cfg.m;
		const uint	cols_c	= cfg.n;

		// A, B, C, R:		R = A * B + C
		// M, N, K:			A[MxK], B[KxN], C[MxN], R[MxN]
		CHECK( cols_a == rows_b );	// K tile
		CHECK( rows_a == rows_c );	// M tile
		CHECK( cols_b == cols_c );	// N tile

		// large matrix which divided on tiles
		const uint	num_tiles_m	= DivCeil( M, cfg.m );
		const uint	num_tiles_n	= DivCeil( N, cfg.n );
		const uint	num_tiles_k	= DivCeil( K, cfg.k );
		const uint	sg_tiles_m	= IsMultipleOf( num_tiles_m, 2 ) ? 2 : 1;
		const uint	sg_tiles_n	= IsMultipleOf( num_tiles_n, 2 ) ? 2 : 1;

		Executor::WGConfig	wg_cfg;
		wg_cfg.subgroupCount	= sg_tiles_m * sg_tiles_n;	// 1..4
		wg_cfg.wgCount.x		= num_tiles_m / sg_tiles_m;
		wg_cfg.wgCount.y		= num_tiles_n / sg_tiles_n;

		// to avoid out-of-bounds read
		CHECK( IsMultipleOf( M, cfg.m ));
		CHECK( IsMultipleOf( N, cfg.n ));
		CHECK( IsMultipleOf( K, cfg.k ));

		const bool	is_fp32 = cfg.c == Graphics::ECoopMatrixComponentType::Float32;
		CHECK( sizeof(CType) == (is_fp32 ? 4 : 2) );

		for (uint t = 0; t < 1; ++t)
		{
			const bool		col_major	= true;

			String			src;
			Array<half>		input_a, input_b;
			Array<CType>	input_c, output;

			input_a.resize( M * K, half::Zero() );
			input_b.resize( K * N, half::Zero() );
			input_c.resize( M * N, CType(0.f) );
			output .resize( M * N, CType(0.f) );

			FillWithLinearData( input_a, 0.001f,	1.f );
			FillWithLinearData( input_b, 0.01f,		-10.f );
			FillWithLinearData( input_c, 0.02f,		0.1f );

			src << "#define type_ab		half\n"
				<< "#define type_c		" << (is_fp32 ? "float" : "half") << '\n'
				<< "#define c_scale		" << (is_fp32 ? "2" : "1") << '\n'		// because 'ex.Run()' gets 'sizeof(half)' - 2bytes
				<< "#define rows_a		" << ToString( rows_a ) << '\n'
				<< "#define cols_a		" << ToString( cols_a ) << '\n'
				<< "#define rows_b		" << ToString( rows_b ) << '\n'
				<< "#define cols_b		" << ToString( cols_b ) << '\n'
				<< "#define rows_c		" << ToString( rows_c ) << '\n'
				<< "#define cols_c		" << ToString( cols_c ) << '\n'
				<< "#define layout		gl::CooperativeMatrixLayout::" << (col_major ? "ColumnMajor" : "RowMajor") << '\n'
				<< "#define COL_MAJOR   " << ToString( col_major ? "1" : "0") << '\n'
				<< "#define stride_a	" << ToString( col_major ? M : K ) << '\n'
				<< "#define stride_b	" << ToString( col_major ? K : N ) << '\n'
				<< "#define stride_c	" << ToString( (col_major ? M : N) * (is_fp32 ? 2 : 1) ) << '\n'
				<< "#define mat_m       " << ToString( M ) << '\n'
				<< "#define mat_n       " << ToString( N ) << '\n'
				<< "#define mat_k       " << ToString( K ) << '\n'
				<< "#define sg_tiles_n	" << ToString( sg_tiles_n ) << '\n'
				<< "#define sg_tiles_m	" << ToString( sg_tiles_m ) << '\n'
				<< "#define num_tiles_k	" << ToString( num_tiles_k ) << '\n';

			src << R"(
				#define				OUT
				#define CoopMatA	gl::CoopMat< type_ab, gl::Scope::Subgroup, rows_a, cols_a, gl::MatrixUse::A >
				#define CoopMatB	gl::CoopMat< type_ab, gl::Scope::Subgroup, rows_b, cols_b, gl::MatrixUse::B >
				#define CoopMatC	gl::CoopMat< type_c,  gl::Scope::Subgroup, rows_c, cols_c, gl::MatrixUse::C >

				void Main ()
				{
					CoopMatA	a;
					CoopMatB	b;
					CoopMatC	c;

					uint	tile_n		= gl.subgroup.GroupIndex % sg_tiles_n + gl.WorkGroupID.y * sg_tiles_n;
					uint	tile_m		= gl.subgroup.GroupIndex / sg_tiles_n + gl.WorkGroupID.x * sg_tiles_m;

					uint	offset_n	= cols_b * tile_n;
					uint	offset_m	= rows_a * tile_m;

					// in elements
				  #if COL_MAJOR
					uint	first_c		= (offset_n * mat_m + offset_m) * c_scale;
				  #else
					uint	first_c		= (offset_m * mat_n + offset_n) * c_scale;
				  #endif

					gl.CoopMatLoad( OUT c, un_InputC.data, first_c, stride_c, layout );

					for (uint tile_k = 0; tile_k < num_tiles_k; ++tile_k)
					{
						uint	k		= tile_k * cols_a;

						// in elements
					  #if COL_MAJOR
						uint	first_a	= k * mat_m + offset_m;
						uint	first_b	= offset_n * mat_k + k;
					  #else
						uint	first_a	= offset_m * mat_k + k;
						uint	first_b = k * mat_n + offset_n;
					  #endif

						gl.CoopMatLoad( OUT a, un_InputA.data, first_a, stride_a, layout );
						gl.CoopMatLoad( OUT b, un_InputB.data, first_b, stride_b, layout );

						c = gl.CoopMatMulAdd( a, b, c, 0 );
					}

					// whole subgroup must store data
					gl.CoopMatStore( c, OUT un_Output.data, first_c, stride_c, layout );
				}
			)";
			CHECK_FATAL( ex.Run( src, BufCast(input_a), BufCast(input_b), BufCast(input_c), BufCast(output), sizeof(half), wg_cfg ));


			TestMatMulAdd<half, half, CType>( input_a, input_b, input_c, output, MatrixDim{K, M}, MatrixDim{N, K}, MatrixDim{N, M}, col_major );
		}
	}


	static void  CoopMat_Test3 (Executor &ex)
	{
		using namespace AE::Graphics;

		const uint	M = 64;
		const uint	N = M;
		const uint	K = M;

		auto	mat_bits = GraphicsScheduler().GetFeatureSet().cooperativeMatrixConfig;
		for (auto e : mat_bits)
		{
			CoopMatrixConfig	cfg{e};

			if ( cfg.a != ECoopMatrixComponentType::Float16 or
				 cfg.b != ECoopMatrixComponentType::Float16 or
				 cfg.c != cfg.res )
				continue;

			AE_LOGI( "Test config: "s << ToString(e) );

			switch ( cfg.c )
			{
				case ECoopMatrixComponentType::Float16 :
					CoopMat_Test3Impl<half>( ex, cfg, M, N, K );
					break;

				case ECoopMatrixComponentType::Float32 :
					CoopMat_Test3Impl<float>( ex, cfg, M, N, K );
					break;
			}
		}
	}


	void  Kernel16x16 (ArrayView<float> inputA, ArrayView<float> inputB, ArrayView<float> inputC, MutableArrayView<float> output,
						const MatrixDim dimA, const MatrixDim dimB, const MatrixDim dimC, const bool columnMajor,
						const uint offsetM, const uint offsetN)
	{
		ASSERT( inputA.size() == dimA.rows * dimA.columns );
		ASSERT( inputB.size() == dimB.rows * dimB.columns );
		ASSERT( inputC.size() == dimC.rows * dimC.columns );

		ASSERT( dimA.rows	 == dimC.rows );	// M
		ASSERT( dimA.columns == dimB.rows );	// K
		ASSERT( dimB.columns == dimC.columns );	// N

		const uint	K = dimA.columns;

		if ( columnMajor )
		{
			for (uint i = 0; i < 16; ++i)	// M
			{
				for (uint j = 0; j < 16; ++j)	// N
				{
					const uint	j2	= j + offsetN;
					const uint	i2	= i + offsetM;

					const uint	idx  = j2 * dimC.rows + i2;
					float		sum  = float(inputC[ idx ]);

					for (uint k = 0; k < K; ++k)
					{
						float	a = float(inputA[ k  * dimA.rows + i2 ]);		// col
						float	b = float(inputB[ j2 * dimB.rows + k  ]);		// row

						sum += a * b;
					}

					output[ idx ] = sum;
				}
			}
		}
		else // row-major
		{
			for (uint i = 0; i < 16; ++i)	// M
			{
				for (uint j = 0; j < 16; ++j)	// N
				{
					const uint	j2	 = j + offsetN;
					const uint	i2	 = i + offsetM;
					const uint	idx  = i2 * dimC.columns + j2;
					float		sum  = float(inputC[ idx ]);

					for (uint k = 0; k < K; ++k)
					{
						float	a = float(inputA[ i2 * dimA.columns + k  ]);		// row
						float	b = float(inputB[ k  * dimB.columns + j2 ]);		// col

						sum += a * b;
					}

					output[ idx ] = sum;
				}
			}
		}
	}

	static void  CoopMat_Test3Ref ()
	{
		const uint	M = 64;
		const uint	N = 32;
		const uint	K = 32;
		const uint	tile_size_m	= 16;
		const uint	tile_size_n	= tile_size_m;

		const uint	num_tiles_m	= DivCeil( M, tile_size_m );
		const uint	num_tiles_n	= DivCeil( N, tile_size_n );
		const uint	wg_sg_count	= num_tiles_m * num_tiles_n;

		Array<float>	input_a, input_b;
		Array<float>	input_c, output;

		input_a.resize( M * K, 0.f );
		input_b.resize( K * N, 0.f );
		input_c.resize( M * N, 0.f );
		output .resize( M * N, 0.f );

		for (uint t = 0; t < 2; ++t)
		{
			const bool	col_major	= t > 0;

			FillWithLinearData( input_a, 0.001f,	1.f );
			FillWithLinearData( input_b, 0.01f,		-10.f );
			FillWithLinearData( input_c, 0.02f,		0.1f );

			for (uint id = 0; id < wg_sg_count; ++id)
			{
				uint	tile_n	= id % num_tiles_n;
				uint	tile_m	= id / num_tiles_n;

				Kernel16x16( input_a, input_b, input_c, INOUT output, MatrixDim{K, M}, MatrixDim{N, K}, MatrixDim{N, M}, col_major, tile_size_m * tile_m, tile_size_n * tile_n );
			}

			TestMatMulAdd<float, float, float>( input_a, input_b, input_c, output, MatrixDim{K, M}, MatrixDim{N, K}, MatrixDim{N, M}, col_major );
		}
	}

} // namespace


extern void Test_CoopMat (Executor &ex)
{
	if ( not ex.SupportsCoopMatrix() )
		return;

	CoopMat_Test1( ex );
	CoopMat_Test2( ex );	// only load/store to check offsets
	CoopMat_Test3Ref();		// CPU implementation
	CoopMat_Test3( ex );

	TEST_PASSED();
}
