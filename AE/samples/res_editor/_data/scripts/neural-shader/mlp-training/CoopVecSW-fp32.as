// Copyright (c) Zhirnov Andrey. For more information see 'LICENSE'
#ifdef __INTELLISENSE__
# 	include <res_editor.as>
#	include <glsl.h>
#endif
//-----------------------------------------------------------------------------
#ifdef SCRIPT

	void ASmain ()
	{
		// initialize
		RC<Image>		rt		= Image( EPixelFormat::RGBA16F, SurfaceSize() );
		RC<Buffer>		cbuf	= Buffer();

		// init
		{
			const array<float>	layer1_weight = {
				-0.0928f,  0.0139f,  0.4922f, -0.0628f,
				-0.7881f,  0.8369f, -0.0995f, -0.5659f,
				-0.6118f, -0.0479f,  0.4529f,  1.0029f,
				 0.1842f,  0.1786f,  0.3518f,  0.4033f,
				-0.1718f,  0.7925f,  0.0148f,  0.6353f,
				 0.0883f, -0.1793f,  0.3452f,  0.7446f,
				-0.4482f, -0.8584f, -0.9048f, -0.3621f,
				-0.1393f,  0.9072f, -0.4216f,  0.5576f,
				-0.5093f,  0.7363f,  0.1106f,  0.0964f,
				 0.4783f,  0.1877f, -0.2881f, -0.6841f,
				-0.6191f, -0.2073f,  0.0620f, -0.2289f,
				 0.4561f,  0.4419f,  1.0430f,  0.5889f,
				 0.3206f,  0.6357f, -0.4717f,  0.4082f,
				 0.3484f, -0.7026f,  0.2219f,  0.3301f,
				 0.0861f, -1.1680f, -0.1084f, -0.7729f,
				 0.8970f,  0.8696f, -0.5347f,  0.2196f
			};
			const array<float>	layer1_bias = {
				 0.7134f,
				 0.9053f,
				-0.2942f,
				-1.0752f,
				-0.2935f,
				 0.5703f,
				-0.1776f,
				-0.2017f,
				 0.8032f,
				 0.0580f,
				 0.3833f,
				 0.3315f,
				 0.5352f,
				 0.6489f,
				 0.5229f,
				-0.0424f
			};
			const array<float>	layer2_weight = {
				 0.3367f,  0.3921f, -0.4937f, -0.9429f,
				 0.1268f, -0.4663f,  0.1044f,  0.2329f,
				-0.2917f,  0.9966f, -0.2367f,  0.4331f,
				-0.1051f,  0.3770f, -0.3362f,  0.2964f,
				 0.4985f, -0.8521f, -0.2500f,  0.1519f,
				-0.1794f,  0.0366f, -0.4055f, -0.0549f,
				-0.3408f,  0.0943f, -0.1816f,  0.3987f,
				 0.4573f, -0.0176f, -0.3452f,  0.7578f,
				-0.4170f, -0.2264f, -0.1010f, -0.0543f,
				 0.0832f, -0.0303f, -0.6582f, -0.0494f,
				 0.8413f,  0.0787f, -0.0286f,  0.5908f,
				-0.5493f, -0.4963f, -0.1473f,  0.2625f,
				-0.1842f, -1.1016f, -0.3655f,  0.2754f,
				 0.4307f, -0.5361f, -0.2419f, -0.1438f,
				 0.4321f,  0.5103f,  0.1064f,  0.2695f,
				 0.8521f,  0.5054f, -0.3196f, -0.3792f
			};
			const array<float>	layer2_bias = {
				-0.1865f,
				 0.5977f,
				 0.3621f,
				 0.3047f
			};

			cbuf.FloatArray( "layer1_weight",	layer1_weight );
			cbuf.FloatArray( "layer1_bias",		layer1_bias );

			cbuf.FloatArray( "layer2_weight",	layer2_weight );
			cbuf.FloatArray( "layer2_bias",		layer2_bias );
		}

		// render loop
		{
			RC<ComputePass>		pass = ComputePass();
			pass.ArgOut(	"un_Image",		rt );
			pass.ArgIn(		"un_MlpBuf",	cbuf );
			pass.Slider(	"iCmp",			0,		2,		1 );
			pass.Slider(	"iCmpScale",	-2,		10,		-2 );
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
	float4  groundtruth (float2 v)
	{
		return half4(
			(v.x + v.y) / (1.0 + v.y * v.y),
			2.0 * v.x + v.y,
			0.5 * v.x * v.x + 1.2 * v.y,
			v.x + 0.5 * v.y * v.y
		);
	}


	void  CoopVecMatMulAdd (out float vOutput[16], in float vInput[4], in float vMatrix[16*4], in float vBias[16])
	{
		for (uint i = 0; i < 16; ++i)
		{
			float	sum = vBias[i];

			for (uint j = 0; j < 4; ++j) {
				sum += vMatrix[ i * 4 + j ] * vInput[j];
			}

			vOutput[i] = sum;
		}
	}

	void  CoopVecMatMulAdd (out float vOutput[4], in float vInput[16], in float vMatrix[16*4], in float vBias[4])
	{
		for (uint i = 0; i < 4; ++i)
		{
			float	sum = vBias[i];

			for (uint j = 0; j < 16; ++j) {
				sum += vMatrix[ i * 16 + j ] * vInput[j];
			}

			vOutput[i] = sum;
		}
	}


	float4  Eval (float2 v)
	{
		float	in_vec[4]	= { v.x, v.y, v.x * v.x, v.y * v.y };
		float	layer1_out[16];

		CoopVecMatMulAdd(	OUT layer1_out,
							in_vec,
							un_MlpBuf.layer1_weight,
							un_MlpBuf.layer1_bias );

		// ReLU activation
		for (uint i = 0; i < 16; ++i) {
			layer1_out[i] = Max( layer1_out[i], layer1_out[i] * 0.001hf );
		}

		float	layer2_out[4];

		CoopVecMatMulAdd(	OUT layer2_out,
							layer1_out,
							un_MlpBuf.layer2_weight,
							un_MlpBuf.layer2_bias );

		// ReLU activation
		for (uint i = 0; i < 4; ++i) {
			layer2_out[i] = Max( layer2_out[i], layer2_out[i] * 0.001hf );
		}

		return float4( layer2_out[0], layer2_out[1], layer2_out[2], layer2_out[3] );
	}


	void  Main ()
	{
		float2	uv			= GetGlobalCoordUNorm().xy;
		float4	ref_color	= groundtruth( uv );
		float4	mlp_color	= Eval( uv );
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
