// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		Assert( GetFeatureSet().hasCooperativeVector(), "required 'CooperativeVector' feature" );

		// initialize
		RC<Image>		rt			= Image( EPixelFormat::RGBA16F, SurfaceSize() );
		RC<Buffer>		mlp_buf		= Buffer( "export/mlp-4x16-fp16-0.bin" );
		RC<Buffer>		mlp_buf2	= Buffer( 1 << 20 );
		RC<Buffer>		cbuf		= Buffer();
		ulong			addr_fp16	= mlp_buf.DeviceAddress();
		ulong			addr_fp8	= mlp_buf2.DeviceAddress();

		cbuf.AddReference( mlp_buf );
		cbuf.AddReference( mlp_buf2 );

		cbuf.ULong( "layer1_weight",	addr_fp8 );		addr_fp8  += 512;	addr_fp16 += 128;
		cbuf.ULong( "layer1_bias",		addr_fp16 );						addr_fp16 += 64;
		cbuf.ULong( "layer2_weight",	addr_fp8 );		addr_fp8  += 512;	addr_fp16 += 128;
		cbuf.ULong( "layer2_bias",		addr_fp16 );						addr_fp16 += 64;

		RC<DynamicUInt>		p_mode		= DynamicUInt();
		RC<DynamicUInt>		p_cmp		= DynamicUInt();
		RC<DynamicInt>		p_scale		= DynamicInt();
		const uint			last_mode	= 1;

		Slider( p_mode,		"Format",	0,		last_mode );
		Slider(	p_cmp,		"Cmp",		0,		2,		1 );
		Slider(	p_scale,	"CmpScale",	-2,		10,		-2 );

		const array<string>	mode_str = {
			"FP8e4m3", "FP8e5m2"
		};
		const array<ECoopMatrixComponentType>	types = {
			ECoopMatrixComponentType::Float8_E4M3,
			ECoopMatrixComponentType::Float8_E5M2
		};
		Assert( mode_str.size() == last_mode+1 );
		Assert( types.size() == last_mode+1 );

		// render loop
		for (uint i = 0; i <= last_mode; ++i)
		{
			ECoopMatrixComponentType	src_type	= ECoopMatrixComponentType::Float16;
			ECoopMatrixComponentType	dst_type	= types[i];

			ConvertCooperativeVectorMatrix(
				16, 4,
				src_type, mlp_buf,  0,		128, 4*2, ECoopVecMatrixLayout::RowMajor,			// src: half16x4
				dst_type, mlp_buf2, 0,		512, 0,   ECoopVecMatrixLayout::InferencingOptimal	// dst: fp8 inferencing optimal
			);
			ConvertCooperativeVectorMatrix(
				4, 16,
				src_type, mlp_buf,  192,	128, 16*2, ECoopVecMatrixLayout::RowMajor,			// src: half4x16
				dst_type, mlp_buf2, 512,	512, 0,    ECoopVecMatrixLayout::InferencingOptimal	// dst: fp8 inferencing optimal
			);

			RC<ComputePass>		pass = ComputePass( "", "MODE_"s + mode_str[i] );
			pass.ArgOut(	"un_Image",		rt );
			pass.ArgIn(		"un_MlpBuf",	cbuf );
			pass.Constant(	"iCmp",			p_cmp );
			pass.Constant(	"iCmpScale",	p_scale );
			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( rt.Dimension() );
			pass.EnableIfEqual( p_mode, i );
		}

		Present( rt );
	}

#endif
//-----------------------------------------------------------------------------
#ifdef SH_COMPUTE
	#include "InvocationID.glsl"


	// function which will be approximated
	half4  groundtruth (half2 v)
	{
		return half4(
			(v.x + v.y) / (1.0 + v.y * v.y),
			2.0 * v.x + v.y,
			0.5 * v.x * v.x + 1.2 * v.y,
			v.x + 0.5 * v.y * v.y
		);
	}


	layout(std430, buffer_reference) buffer MLPMatrix { half data[]; };

	half4  Eval (half2 v)
	{
		#define CoopVec4	gl::CoopVec< half, 4 >
		#define CoopVec16	gl::CoopVec< half, 16 >

	  #ifdef MODE_FP8e4m3
		const gl::ComponentType		comp_type = gl::ComponentType::FloatE4M3;
	  #endif
	  #ifdef MODE_FP8e5m2
		const gl::ComponentType		comp_type = gl::ComponentType::FloatE5M2;
	  #endif
		const gl::ComponentType		bias_type = gl::ComponentType::Float16;

		CoopVec4	in_vec;
		in_vec[0] = v.x;
		in_vec[1] = v.y;
		in_vec[2] = v.x * v.x;
		in_vec[3] = v.y * v.y;

		MLPMatrix	layer1_weight	= MLPMatrix(un_MlpBuf.layer1_weight);	// [16x4]
		MLPMatrix	layer1_bias		= MLPMatrix(un_MlpBuf.layer1_bias);		// [16]
		CoopVec16	layer1_out;

		gl.CoopVecMatMulAdd(	OUT layer1_out,								// output [16]
								in_vec, comp_type,							// input [4]
								layer1_weight.data, 0, comp_type,			// matrix [16x4]
								layer1_bias.data, 0, bias_type,				// bias [16]
								16, 4,
								gl::CoopVectorMatrixLayout::InferencingOptimal,
								false,										// transpose
								0											// matrixStride
							);

		// ReLU activation
		layer1_out = Max( layer1_out, layer1_out * 0.001hf );

		MLPMatrix	layer2_weight	= MLPMatrix(un_MlpBuf.layer2_weight);	// [4x16]
		MLPMatrix	layer2_bias		= MLPMatrix(un_MlpBuf.layer2_bias);		// [4]
		CoopVec4	layer2_out;

		gl.CoopVecMatMulAdd(	OUT layer2_out,								// output [4]
								layer1_out, comp_type,						// input [16]
								layer2_weight.data, 0, comp_type,			// matrix [4x16]
								layer2_bias.data, 0, bias_type,				// bias [4]
								4, 16,
								gl::CoopVectorMatrixLayout::InferencingOptimal,
								false,										// transpose
								0											// matrixStride
							);

		// ReLU activation
		layer2_out = Max( layer2_out, layer2_out * 0.001hf );

		return half4( layer2_out[0], layer2_out[1], layer2_out[2], layer2_out[3] );
	}


	void  Main ()
	{
		float2	uv			= GetGlobalCoordUNorm().xy;
		float4	ref_color	= float4(groundtruth( half2(uv) ));
		float4	mlp_color	= float4(Eval( half2(uv) ));
		float4	color;

		switch ( iCmp )
		{
			case 0 :	color = ref_color;	break;
			case 1 :	color = mlp_color;	break;
			case 2 :	color = Abs( ref_color - mlp_color ) * Exp2( float(iCmpScale) );break;
		}

		gl.image.Store( un_Image, GetGlobalCoord().xy, color );
	}

#endif
//-----------------------------------------------------------------------------
