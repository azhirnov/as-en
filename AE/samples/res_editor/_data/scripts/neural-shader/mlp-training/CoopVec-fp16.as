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
		RC<Buffer>		cbuf		= Buffer();
		ulong			addr		= mlp_buf.DeviceAddress();

		cbuf.AddReference( mlp_buf );

		cbuf.ULong( "layer1_weight",	addr );		addr += 128;
		cbuf.ULong( "layer1_bias",		addr );		addr += 64;
		cbuf.ULong( "layer2_weight",	addr );		addr += 128;
		cbuf.ULong( "layer2_bias",		addr );		addr += 64;

		RC<DynamicUInt>		p_cmp		= DynamicUInt();
		RC<DynamicUInt>		p_scale		= DynamicUInt();
		const uint			last_mode	= 2;

		Slider(	p_cmp,		"Cmp",		0,		2,		1 );
		Slider(	p_scale,	"CmpScale",	-2,		10,		-2 );

		// render loop
		{
			RC<ComputePass>		pass = ComputePass();
			pass.ArgOut(	"un_Image",		rt );
			pass.ArgIn(		"un_MlpBuf",	cbuf );
			pass.Constant(	"iCmp",			p_cmp );
			pass.Constant(	"iCmpScale",	p_scale );
			pass.LocalSize( 8, 8 );
			pass.DispatchThreads( rt.Dimension() );
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

		const gl::ComponentType		comp_type = gl::ComponentType::Float16;

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
								layer1_bias.data, 0, comp_type,				// bias [16]
								16, 4,
								gl::CoopVectorMatrixLayout::RowMajor,
								false,										// transpose
								4*2											// matrixStride
							);

		// ReLU activation
		layer1_out = Max( layer1_out, layer1_out * 0.001hf );

		MLPMatrix	layer2_weight	= MLPMatrix(un_MlpBuf.layer2_weight);	// [4x16]
		MLPMatrix	layer2_bias		= MLPMatrix(un_MlpBuf.layer2_bias);		// [4]
		CoopVec4	layer2_out;

		gl.CoopVecMatMulAdd(	OUT layer2_out,								// output [4]
								layer1_out, comp_type,						// input [16]
								layer2_weight.data, 0, comp_type,			// matrix [4x16]
								layer2_bias.data, 0, comp_type,				// bias [4]
								4, 16,
								gl::CoopVectorMatrixLayout::RowMajor,
								false,										// transpose
								16*2										// matrixStride
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
