// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
/*
	references:
	* [Benchmark](https://github.com/jeffbolznv/vk_cooperative_matrix_perf)
*/
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	define SH_COMPUTE
#	define AE_cooperative_matrix
#	define AE_memory_scope_semantics
#	include <glsl.h>
#	define M		16
#	define N		16
#	define K		16
#	define CTYPE	float
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<FeatureSet>		fs			= GetFeatureSet();
		const bool			nv_type		= fs.hasCooperativeMatrixConfig( ECoopMatrixCfg::Afp16_Bfp16_Cfp16_Rfp16_M16_N16_K16 );
		const bool			intel_type	= fs.hasCooperativeMatrixConfig( ECoopMatrixCfg::Afp16_Bfp16_Cfp32_Rfp32_M8_N8_K16 );
		Assert( nv_type or intel_type, "unsupported cooperative matrix config" );

		const uint			M			= nv_type ? 16 : (intel_type ? 8 : 0);
		const uint			N			= nv_type ? 16 : (intel_type ? 8 : 0);
		const uint			K			= nv_type ? 16 : (intel_type ? 16 : 0);
		const uint			n_muls		= M * N * K;
		const uint			n_adds		= M * N * (K - 1);

		const uint			dim			= 4<<10;
		const uint			iter_cnt	= 1<<4;

		RC<Image>			rt			= Image( EPixelFormat::RGBA8_UNorm, uint2(dim) );
		RC<DynamicUInt>		count		= DynamicUInt();
		RC<DynamicFloat>	ops			= DynamicFloat( float(dim * dim) * float(iter_cnt) * float(n_muls + n_adds) * 1.0e-12 );
		RC<DynamicFloat>	time		= DynamicFloat();
		RC<DynamicFloat>	flops		= ops.Div( time );

		string				def;
		def += ("COUNT=" + iter_cnt + "; ");
		def += ("M="s + M + "; ");
		def += ("N="s + N + "; ");
		def += ("K="s + K + "; ");
		def += "CTYPE=" + (nv_type ? "half" : (intel_type ? "float" : "void")) + "; ";

		Slider( count,	"Repeat",	1,	32 );
		Label(  flops,	"TOPS" );

		// render loop
		{
			RC<ComputePass>		pass = ComputePass( "", def );
			pass.ArgOut( "un_OutImage", rt );
			pass.LocalSize( GetSubgroupSize(), 1 );
			pass.DispatchThreads( rt.Dimension2() );
			pass.Repeat( count );
			pass.MeasureTime( time );
		}
		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
	#include "InvocationID.glsl"

	#define CoopMatA	gl::CoopMat< half,  gl::Scope::Subgroup, M, K, gl::MatrixUse::A >
	#define CoopMatB	gl::CoopMat< half,  gl::Scope::Subgroup, K, N, gl::MatrixUse::B >
	#define CoopMatC	gl::CoopMat< CTYPE, gl::Scope::Subgroup, M, N, gl::MatrixUse::C >


	void  Main ()
	{
		// matrices are shared for whole subgroup
		CoopMatA	s_MatA = CoopMatA( 1.0hf );
		CoopMatB	s_MatB = CoopMatB( 2.0hf );
		CoopMatC	s_MatC = CoopMatC( CTYPE(3.0) );
		CoopMatC	s_MatR;
		const uint	j = gl.subgroup.Index;

		[[unroll]] for (uint i = 0; i < COUNT; ++i)
		{
			s_MatR = gl.CoopMatMulAdd( s_MatA, s_MatB, s_MatC );

			// latency can be hidden
			CTYPE	a = s_MatR[ (i + j + 1) % s_MatR.length() ];
			CTYPE	b = s_MatR[ (i + j + 2) % s_MatR.length() ];
			CTYPE	c = s_MatR[ (i + j + 3) % s_MatR.length() ];

			s_MatA[ (i + j) % s_MatA.length() ] += half(a);
			s_MatB[ (i + j) % s_MatB.length() ] += half(b);
			s_MatC[ (i + j) % s_MatC.length() ] += c;
		}

		float4	col = float4( s_MatR[ (j*4+0) % s_MatR.length() ], s_MatR[ (j*4+1) % s_MatR.length() ],
							  s_MatR[ (j*4+2) % s_MatR.length() ], s_MatR[ (j*4+3) % s_MatR.length() ] );
		gl.image.Store( un_OutImage, GetGlobalCoord().xy, col );
	}

#endif
//-----------------------------------------------------------------------------
