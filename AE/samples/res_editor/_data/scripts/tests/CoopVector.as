// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Doesn't make any effect, just compiles.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	define AE_ENABLE_HALF_TYPE
#	define AE_cooperative_vector
#	define AE_cooperative_vector_training
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>	rt	= Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );		rt.Name( "RT" );
		RC<Buffer>	buf	= Buffer();

		array<float>	matrix;
		matrix.resize( 16*16 );

		buf.FloatArray( "matrix",	matrix );

		// render loop
		{
			RC<ComputePass>		pass = ComputePass();
			pass.ArgOut( "un_OutImage", rt );
			pass.ArgIn(  "un_CBuf",		buf );
			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( rt.Dimension() );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
	#include "InvocationID.glsl"

	void  Main ()
	{
		gl::CoopVec< half, 16 >		a = gl::CoopVec< half, 16 >( 1.0hf );
		gl::CoopVec< half, 16 >		b = gl::CoopVec< half, 16 >( 2.0hf );

		a *= 0.9hf;

		a += b;
		a *= b;
		
		gl::CoopVec< half, 16 >		c;
		gl.CoopVecMatMul(	OUT c,
							a, gl::ComponentType::Float16,					// input
							un_CBuf.matrix, 0, gl::ComponentType::Float32,	// matrix
							16, 16,											// M x K
							gl::CoopVectorMatrixLayout::ColumnMajor,
							false,											// transpose
							0												// matrixStride
						);

		#ifdef AE_cooperative_vector_training
		{
			//gl.CoopVecOuterProductAccum();
			//gl.CoopVecReduceSumAccum();
		}
		#endif

		gl.image.Store( un_OutImage, GetGlobalCoord().xy, float4(0.0) );
	}

#endif
//-----------------------------------------------------------------------------
