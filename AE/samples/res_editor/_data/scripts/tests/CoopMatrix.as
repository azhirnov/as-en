// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	Doesn't make any effect, just compiles.
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	define SH_COMPUTE
#	define AE_cooperative_matrix
#	define AE_memory_scope_semantics
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>	rt = Image( EPixelFormat::RGBA8_UNorm, SurfaceSize() );		rt.Name( "RT" );

		// render loop
		{
			RC<ComputePass>		pass = ComputePass();
			pass.ArgOut( "un_OutImage", rt );
			pass.LocalSize( GetSubgroupSize(), 1 );
			pass.DispatchThreads( rt.Dimension() );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
	#include "InvocationID.glsl"
	
	#if defined(AE_NVidia_GPU) or defined(AE_AMD_GPU)
		#define CoopMatA	gl::CoopMat< half, gl::Scope::Subgroup, 16, 16, gl::MatrixUse::A >
		#define CoopMatB	gl::CoopMat< half, gl::Scope::Subgroup, 16, 16, gl::MatrixUse::B >
		#define CoopMatC	gl::CoopMat< half, gl::Scope::Subgroup, 16, 16, gl::MatrixUse::C >
	#endif
	#if defined(AE_Intel_GPU)
		#define CoopMatA	gl::CoopMat< half, gl::Scope::Subgroup,  8, 32, gl::MatrixUse::A >
		#define CoopMatB	gl::CoopMat< half, gl::Scope::Subgroup, 32, 8,  gl::MatrixUse::B >
		#define CoopMatC	gl::CoopMat< half, gl::Scope::Subgroup,  8, 8,  gl::MatrixUse::C >
	#endif

	void  Main ()
	{
		// matrices are shared for whole subgroup
		CoopMatA	s_MatA = CoopMatA( 1.0hf );
		CoopMatB	s_MatB = CoopMatB( 2.0hf );
		CoopMatC	s_MatC = CoopMatC( 3.0hf );
		CoopMatC	s_MatR;

		s_MatR = gl.CoopMatMulAdd( s_MatA, s_MatB, s_MatC );

		s_MatC = s_MatC + s_MatR;

		gl.image.Store( un_OutImage, GetGlobalCoord().xy, float4(0.0) );
	}

#endif
//-----------------------------------------------------------------------------
