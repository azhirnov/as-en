// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	define AE_ENABLE_HALF_TYPE
#	define AE_cooperative_vector
#	define AE_cooperative_vector_training
#	include <glsl.h>

#	define FMT_HALF
#	define FMT_FP8_E4M3
#	define FMT_FP8_E5M2
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		const uint			dim			= 4<<10;
		const uint			iter_cnt	= 1<<4;
		const uint			vec_size	= 64;	// must be >= 64 to hide latency for 'a += c'

		RC<Image>			rt			= Image( EPixelFormat::RGBA8_UNorm, uint2(dim) );
		RC<Buffer>			buf			= Buffer();
		RC<DynamicUInt>		count		= DynamicUInt();
		RC<DynamicUInt>		mode		= DynamicUInt();
		RC<DynamicFloat>	ops			= DynamicFloat( float(dim * dim) * float(iter_cnt) * float(vec_size * vec_size) * 1.0e-12 );
		RC<DynamicFloat>	time		= DynamicFloat();
		RC<DynamicFloat>	flops		= ops.Div( time );

		array<float>	matrix;
		matrix.resize( vec_size * vec_size );

		for (uint y = 0; y < vec_size; ++y)
		for (uint x = 0; x < vec_size; ++x)
			matrix[x + y*vec_size] = (x == y ? 1.0 : 0.0);

		buf.FloatArray( "matrix",	matrix );

		Slider( mode,	"Mode",		0,	2 );
		Slider( count,	"Repeat",	1,	32 );
		Label(  flops,	"TOPS" );

		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", "FMT_HALF; COUNT="+iter_cnt+"; VECSIZE="+vec_size );
			pass.SetDebugLabel( "fp16", RGBA32f(1.0, 0.0, 0.0, 1.0) );
			pass.ArgOut( "un_OutImage", rt );
			pass.ArgIn(  "un_CBuf",		buf );
			pass.LocalSize( 16, 16 );
			pass.DispatchThreads( rt.Dimension2() );
			pass.Repeat( count );
			pass.EnableIfEqual( mode, 0 );
			pass.MeasureTime( time );
		}{
			RC<ComputePass>		pass = ComputePass( "", "FMT_FP8_E4M3; COUNT="+iter_cnt+"; VECSIZE="+vec_size );
			pass.SetDebugLabel( "fp8 e4m3", RGBA32f(0.0, 1.0, 0.0, 1.0) );
			pass.ArgOut( "un_OutImage", rt );
			pass.ArgIn(  "un_CBuf",		buf );
			pass.LocalSize( 16, 16 );
			pass.DispatchThreads( rt.Dimension2() );
			pass.Repeat( count );
			pass.EnableIfEqual( mode, 1 );
			pass.MeasureTime( time );
		}{
			RC<ComputePass>		pass = ComputePass( "", "FMT_FP8_E5M2; COUNT="+iter_cnt+"; VECSIZE="+vec_size );
			pass.SetDebugLabel( "fp8 e5m2", RGBA32f(0.0, 0.0, 1.0, 1.0) );
			pass.ArgOut( "un_OutImage", rt );
			pass.ArgIn(  "un_CBuf",		buf );
			pass.LocalSize( 16, 16 );
			pass.DispatchThreads( rt.Dimension2() );
			pass.Repeat( count );
			pass.EnableIfEqual( mode, 2 );
			pass.MeasureTime( time );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef FMT_HALF
	#include "InvocationID.glsl"

	#define VecType		gl::CoopVec< half, VECSIZE >

	void  Main ()
	{
		VecType		a = VecType( 1.0hf );

		[[unroll]] for (uint i = 0; i < COUNT; ++i)
		{
			VecType		c;

			// M*K scalar FMAs
			gl.CoopVecMatMul(	OUT c,
								a, gl::ComponentType::Float16,					// input
								un_CBuf.matrix, 0, gl::ComponentType::Float16,	// matrix
								VECSIZE, VECSIZE,								// M x K
								gl::CoopVectorMatrixLayout::ColumnMajor,
								false,											// transpose
								0												// matrixStride
							);

			// M ADDs (latency can be hidden)
			a += c;
		}
		
		float4	col = float4( a[0], a[1], a[2], a[3] );
		if ( AllLess( col, float4(-1.e+20) ))
			gl.image.Store( un_OutImage, GetGlobalCoord().xy, col );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef FMT_FP8_E4M3
	#include "InvocationID.glsl"

	#define VecType		gl::CoopVec< half, VECSIZE >

	void  Main ()
	{
		VecType		a = VecType( 1.0hf );

		[[unroll]] for (uint i = 0; i < COUNT; ++i)
		{
			VecType		c;

			// M*K scalar FMAs
			gl.CoopVecMatMul(	OUT c,
								a, gl::ComponentType::FloatE4M3,					// input
								un_CBuf.matrix, 0, gl::ComponentType::FloatE4M3,	// matrix
								VECSIZE, VECSIZE,									// M x K
								gl::CoopVectorMatrixLayout::InferencingOptimal,
								false,												// transpose
								0													// matrixStride
							);

			// M ADDs (latency can be hidden)
			a += c;
		}
		
		float4	col = float4( a[0], a[1], a[2], a[3] );
		if ( AllLess( col, float4(-1.e+20) ))
			gl.image.Store( un_OutImage, GetGlobalCoord().xy, col );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef FMT_FP8_E5M2
	#include "InvocationID.glsl"

	#define VecType		gl::CoopVec< half, VECSIZE >

	void  Main ()
	{
		VecType		a = VecType( 1.0hf );

		[[unroll]] for (uint i = 0; i < COUNT; ++i)
		{
			VecType		c;

			// M*K scalar FMAs
			gl.CoopVecMatMul(	OUT c,
								a, gl::ComponentType::FloatE5M2,					// input
								un_CBuf.matrix, 0, gl::ComponentType::FloatE5M2,	// matrix
								VECSIZE, VECSIZE,									// M x K
								gl::CoopVectorMatrixLayout::InferencingOptimal,
								false,												// transpose
								0													// matrixStride
							);

			// M ADDs (latency can be hidden)
			a += c;
		}

		float4	col = float4( a[0], a[1], a[2], a[3] );
		if ( AllLess( col, float4(-1.e+20) ))
			gl.image.Store( un_OutImage, GetGlobalCoord().xy, col );
	}

#endif
//-----------------------------------------------------------------------------
